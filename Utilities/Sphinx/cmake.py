# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Standard imports
import os
import re

# Third party packages
from docutils import io, nodes
from docutils.parsers.rst import Directive, directives
from docutils.transforms import Transform
from pygments.lexer import bygroups
from pygments.lexers import CMakeLexer  # NOQA disable=no-name-in-module
from pygments.token import (
    Comment,
    Name,
    Number,
    Operator,
    Punctuation,
    String,
    Text,
    Whitespace
)
from sphinx import addnodes
from sphinx.directives import ObjectDescription
from sphinx.domains import Domain, ObjType
from sphinx.roles import XRefRole
from sphinx.util.nodes import make_refnode

SPHINX_BEFORE_1_4 = False
SPHINX_BEFORE_1_7_2 = False
try:
    # Third party packages
    from sphinx import version_info
    if version_info < (1, 4):
        SPHINX_BEFORE_1_4 = True
    if version_info < (1, 7, 2):
        SPHINX_BEFORE_1_7_2 = True
except ImportError:
    # The `sphinx.version_info` tuple was added in Sphinx v1.2:
    SPHINX_BEFORE_1_4 = True
    SPHINX_BEFORE_1_7_2 = True

if SPHINX_BEFORE_1_7_2:
    # Monkey patch for sphinx generating invalid content for qcollectiongenerator
    # https://github.com/sphinx-doc/sphinx/issues/1435
    # Third party packages
    from sphinx.builders.qthelp import QtHelpBuilder  # NOQA disable=import-error
    from sphinx.util.pycompat import htmlescape  # NOQA disable=import-error

    old_build_keywords = QtHelpBuilder.build_keywords

    def new_build_keywords(self, title, refs, subitems):
        old_items = old_build_keywords(self, title, refs, subitems)
        new_items = []
        for item in old_items:
            before, rest = item.split('ref="', 1)
            ref, after = rest.split('"')
            if '<' in ref and '>' in ref:
                new_items.append(before + 'ref="' + htmlescape(ref) + '"' + after)
            else:
                new_items.append(item)
        return new_items

    QtHelpBuilder.build_keywords = new_build_keywords

# Override much of pygments' CMakeLexer.
# We need to parse CMake syntax definitions, not CMake code.
#
# For hard test cases that use much of the syntax below, see
# - module/FindPkgConfig.html (with "glib-2.0>=2.10 gtk+-2.0" and similar)
# - module/ExternalProject.html (with http:// https:// git@; also has command options -E --build)
# - manual/cmake-buildsystem.7.html (with nested $<..>; relative and absolute paths, "::")
#
# Notes on regular expressions below:
# - [\.\+-] are needed for string constants like gtk+-2.0
# - Unix paths are recognized by '/'; support for Windows paths may be added if needed
# - (\\.) allows for \-escapes (used in manual/cmake-language.7)
# - $<..$<..$>..> nested occurrence in cmake-buildsystem
# - Nested variable evaluations are only supported in a limited capacity. Only
#   one level of nesting is supported and at most one nested variable can be present.
CMakeLexer.tokens['root'] = [
  (r'\b(\w+)([ \t]*)(\()', bygroups(Name.Function, Text, Name.Function), '#push'),  # fctn(
  (r'\(', Name.Function, '#push'),
  (r'\)', Name.Function, '#pop'),
  (r'\[', Punctuation, '#push'),
  (r'\]', Punctuation, '#pop'),
  (r'[|;,.=*\-]', Punctuation),
  (r'\\\\', Punctuation),                                   # used in commands/source_group
  (r'[:]', Operator),
  (r'[<>]=', Punctuation),                                  # used in FindPkgConfig.cmake
  (r'\$<', Operator, '#push'),                              # $<...>
  (r'<[^<|]+?>(\w*\.\.\.)?', Name.Variable),                # <expr>
  (r'(\$\w*\{)([^\}\$]*)?(?:(\$\w*\{)([^\}]+?)(\}))?([^\}]*?)(\})',  # ${..} $ENV{..}, possibly nested
    bygroups(Operator, Name.Tag, Operator, Name.Tag, Operator, Name.Tag, Operator)),
  (r'([A-Z]+\{)(.+?)(\})', bygroups(Operator, Name.Tag, Operator)),  # DATA{ ...}
  (r'[a-z]+(@|(://))((\\.)|[\w.+-:/\\])+', Name.Attribute),          # URL, git@, ...
  (r'/\w[\w\.\+-/\\]*', Name.Attribute),                    # absolute path
  (r'/', Name.Attribute),
  (r'\w[\w\.\+-]*/[\w.+-/\\]*', Name.Attribute),            # relative path
  (r'[A-Z]((\\.)|[\w.+-])*[a-z]((\\.)|[\w.+-])*', Name.Builtin),  # initial A-Z, contains a-z
  (r'@?[A-Z][A-Z0-9_]*', Name.Constant),
  (r'[a-z_]((\\;)|(\\ )|[\w.+-])*', Name.Builtin),
  (r'[0-9][0-9\.]*', Number),
  (r'(?s)"(\\"|[^"])*"', String),                           # "string"
  (r'\.\.\.', Name.Variable),
  (r'<', Operator, '#push'),                                # <..|..> is different from <expr>
  (r'>', Operator, '#pop'),
  (r'\n', Whitespace),
  (r'[ \t]+', Whitespace),
  (r'#.*\n', Comment),
]


class CMakeModule(Directive):
    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = {'encoding': directives.encoding}

    def __init__(self, *args, **keys):
        self.re_start = re.compile(r'^#\[(?P<eq>=*)\[\.rst:$')
        Directive.__init__(self, *args, **keys)

    def run(self):                                          # NOQA disable=too-many-branches
        settings = self.state.document.settings
        if not settings.file_insertion_enabled:
            raise self.warning('"{0}" directive disabled.'.format(self.name))

        env = self.state.document.settings.env
        _, path = env.relfn2path(self.arguments[0])
        path = os.path.normpath(path)
        encoding = self.options.get('encoding', settings.input_encoding)
        e_handler = settings.input_encoding_error_handler
        try:
            settings.record_dependencies.add(path)
            stream = io.FileInput(source_path=path, encoding=encoding, error_handler=e_handler)
        except UnicodeEncodeError:
            raise self.severe(                              # NOQA disable=raise-missing-from
                'Problems with "{0}" directive path:\n'
                'Cannot encode input file path "{1}" (wrong locale?).'.format(
                    self.name
                  , path
                  )
              )
        except IOError as error:
            raise self.severe(
                'Problems with "{0}" directive path:\n{1}.'.format(
                    self.name
                  , error
                  )
              )
        raw_lines = stream.read().splitlines()
        stream.close()
        rst = None
        lines = []
        for line in raw_lines:
            if rst is not None and rst != '#':
                # Bracket mode: check for end bracket
                pos = line.find(rst)
                if pos >= 0:
                    if line[0] == '#':
                        line = ''
                    else:
                        line = line[0:pos]
                    rst = None
            else:
                # Line mode: check for .rst start (bracket or line)
                match = self.re_start.match(line)
                if match:
                    rst = ']{0}]'.format(match.group('eq'))
                    line = ''
                elif line == '#.rst:':
                    rst = '#'
                    line = ''
                elif rst == '#':
                    if line == '#' or line[:2] == '# ':
                        line = line[2:]
                    else:
                        rst = None
                        line = ''
                elif rst is None:
                    line = ''
            lines.append(line)
        if rst is not None and rst != '#':
            raise self.warning(
                '"{0}" found unclosed bracket "#[{1}[.rst:" in {2}'.format(
                    self.name, rst[1:-1], path
                  )
              )
        self.state_machine.insert_input(lines, path)
        return []


class CMakeIndexEntry:                                      # NOQA disable=too-few-public-methods
    def __init__(self, desc):
        self.desc = desc

    def __call__(self, title, targetid, main='main'):
        # See https://github.com/sphinx-doc/sphinx/issues/2673
        if SPHINX_BEFORE_1_4:
            return ('pair', '{0} ; {1}'.format(self.desc, title), targetid, main)
        return ('pair', '{0} ; {1}'.format(self.desc, title), targetid, main, None)


_cmake_index_objs = {
    'command':    CMakeIndexEntry('command'),
    'cpack_gen':  CMakeIndexEntry('cpack generator'),
    'envvar':     CMakeIndexEntry('envvar'),
    'generator':  CMakeIndexEntry('generator'),
    'genex':      CMakeIndexEntry('genex'),
    'guide':      CMakeIndexEntry('guide'),
    'manual':     CMakeIndexEntry('manual'),
    'module':     CMakeIndexEntry('module'),
    'policy':     CMakeIndexEntry('policy'),
    'prop_cache': CMakeIndexEntry('cache property'),
    'prop_dir':   CMakeIndexEntry('directory property'),
    'prop_gbl':   CMakeIndexEntry('global property'),
    'prop_inst':  CMakeIndexEntry('installed file property'),
    'prop_sf':    CMakeIndexEntry('source file property'),
    'prop_test':  CMakeIndexEntry('test property'),
    'prop_tgt':   CMakeIndexEntry('target property'),
    'variable':   CMakeIndexEntry('variable'),
    }


def _cmake_object_inventory(env, document, line, objtype, targetid):
    inv = env.domaindata['cmake']['objects']
    if targetid in inv:
        document.reporter.warning(
            'CMake object "{0}" also described in "{1}".'.format(
                targetid
              , env.doc2path(inv[targetid][0])
              )
          , line=line
          )
    inv[targetid] = (env.docname, objtype)


class CMakeTransform(Transform):
    # Run this transform early since we insert nodes we want
    # treated as if they were written in the documents.
    default_priority = 210

    def __init__(self, document, startnode):
        Transform.__init__(self, document, startnode)
        self.titles = {}

    def parse_title(self, docname):
        """
            Parse a document title as the first line starting in [A-Za-z0-9<$]
            or fall back to the document basename if no such line exists.
            The cmake `--help-*-list` commands also depend on this convention.
            Return the title or False if the document file does not exist.
        """
        env = self.document.settings.env
        title = self.titles.get(docname)
        if title is None:
            fname = os.path.join(env.srcdir, docname + '.rst')
            try:
                fd = open(fname, 'r')                       # NOQA
            except IOError:
                title = False
            else:
                for line in fd:
                    if len(line) > 0 and (
                        line[0].isalnum() or line[0] == '<' or line[0] == '$'
                      ):
                        title = line.rstrip()
                        break
                fd.close()
                if title is None:
                    title = os.path.basename(docname)
            self.titles[docname] = title
        return title

    def apply(self, *_args, **_kwargs):
        env = self.document.settings.env

        # Treat some documents as cmake domain objects.
        objtype, _, tail = env.docname.partition('/')
        make_index_entry = _cmake_index_objs.get(objtype)
        if make_index_entry:
            title = self.parse_title(env.docname)
            # Insert the object link target.
            if objtype == 'command':
                targetname = title.lower()
            elif objtype == 'guide' and not tail.endswith('/index'):
                targetname = tail
            else:
                if objtype == 'genex':
                    match = CMakeXRefRole.GENEX_RE.match(title)
                    if match:
                        title = match.group(1)
                targetname = title
            targetid = '{0}:{1}'.format(objtype, targetname)
            targetnode = nodes.target('', '', ids=[targetid])
            self.document.note_explicit_target(targetnode)
            self.document.insert(0, targetnode)
            # Insert the object index entry.
            indexnode = addnodes.index()
            indexnode['entries'] = [make_index_entry(title, targetid)]
            self.document.insert(0, indexnode)
            # Add to cmake domain object inventory
            _cmake_object_inventory(env, self.document, 1, objtype, targetid)


class CMakeObject(ObjectDescription):
    def handle_signature(self, sig, signode):
        # called from sphinx.directives.ObjectDescription.run()
        signode += addnodes.desc_name(sig, sig)
        if self.objtype != 'genex':
            return sig

        match = CMakeXRefRole.GENEX_RE.match(sig)
        return match.group(1) if match else sig

    def add_target_and_index(self, name, sig, signode):
        if self.objtype == 'command':
            targetname = name.lower()
        else:
            targetname = name
        targetid = '{0}:{1}'.format(self.objtype, targetname)
        if targetid not in self.state.document.ids:
            signode['names'].append(targetid)
            signode['ids'].append(targetid)
            signode['first'] = not self.names
            self.state.document.note_explicit_target(signode)
            _cmake_object_inventory(
                self.env, self.state.document, self.lineno, self.objtype, targetid
              )

        make_index_entry = _cmake_index_objs.get(self.objtype)
        if make_index_entry:
            self.indexnode['entries'].append(make_index_entry(name, targetid))


class CMakeXRefRole(XRefRole):
    # See sphinx.util.nodes.explicit_title_re; \x00 escapes '<'.
    ANGLE_BRACKETS_RE = re.compile(r'^(.+?)(\s*)(?<!\x00)<(.*?)>$', re.DOTALL)
    SUBCOMMAND_RE = re.compile(r'^([^()\s]+)\s*\(([^()]*)\)$', re.DOTALL)
    GENEX_RE = re.compile(r'^\$<([^<>:]+)(:[^<>]+)?>$', re.DOTALL)
    GUIDE_RE = re.compile(r'^([^<>/]+)/([^<>]*)$', re.DOTALL)

    def __call__(self, typ, rawtext, text, *args, **keys):
        # Translate CMake command cross-references of the form:
        #  `command_name(SUB_COMMAND)`
        # to have an explicit target:
        #  `command_name(SUB_COMMAND) <command_name>`
        if typ == 'cmake:command':
            match = CMakeXRefRole.SUBCOMMAND_RE.match(text)
            if match:
                text = '{0} <{1}>'.format(text, match.group(1))
        elif typ == 'cmake:genex':
            match = CMakeXRefRole.GENEX_RE.match(text)
            if match:
                text = '{0} <{1}>'.format(text, match.group(1))
        elif typ == 'cmake:guide':
            match = CMakeXRefRole.GUIDE_RE.match(text)
            if match:
                text = '{0} <{1}>'.format(match.group(2), text)
        # CMake cross-reference targets frequently contain '<' so escape
        # any explicit `<target>` with '<' not preceded by whitespace.
        while True:
            match = CMakeXRefRole.ANGLE_BRACKETS_RE.match(text)
            if match and len(match.group(2)) == 0:
                text = '{0}\x00<{1}>'.format(match.group(1), match.group(3))
            else:
                break
        return XRefRole.__call__(self, typ, rawtext, text, *args, **keys)

    # We cannot insert index nodes using the result_nodes method
    # because CMakeXRefRole is processed before substitution_reference
    # nodes are evaluated so target nodes (with 'ids' fields) would be
    # duplicated in each evaluated substitution replacement.  The
    # docutils substitution transform does not allow this.  Instead we
    # use our own CMakeXRefTransform below to add index entries after
    # substitutions are completed.
    #
    # def result_nodes(self, document, env, node, is_ref):
    #     pass


class CMakeXRefTransform(Transform):                        # NOQA disable=too-few-public-methods
    # Run this transform early since we insert nodes we want
    # treated as if they were written in the documents, but
    # after the sphinx (210) and docutils (220) substitutions.
    default_priority = 221

    def apply(self, *_args, **_kwargs):
        env = self.document.settings.env

        # Find CMake cross-reference nodes and add index and target
        # nodes for them.
        for ref in self.document.traverse(addnodes.pending_xref):
            if ref['refdomain'] != 'cmake':
                continue

            objtype = ref['reftype']
            make_index_entry = _cmake_index_objs.get(objtype)
            if not make_index_entry:
                continue

            objname = ref['reftarget']
            if objtype == 'guide' and CMakeXRefRole.GUIDE_RE.match(objname):
                # Do not index cross-references to guide sections.
                continue

            targetnum = env.new_serialno('index-{0}:{1}'.format(objtype, objname))

            targetid = 'index-{0}-{1}:{2}'.format(targetnum, objtype, objname)
            targetnode = nodes.target('', '', ids=[targetid])
            self.document.note_explicit_target(targetnode)

            indexnode = addnodes.index()
            indexnode['entries'] = [make_index_entry(objname, targetid, '')]
            ref.replace_self([indexnode, targetnode, ref])


class CMakeDomain(Domain):
    """CMake domain."""
    name = 'cmake'
    label = 'CMake'
    object_types = {
        'command':    ObjType('command',    'command'),
        'cpack_gen':  ObjType('cpack_gen',  'cpack_gen'),
        'envvar':     ObjType('envvar',     'envvar'),
        'generator':  ObjType('generator',  'generator'),
        'genex':      ObjType('genex',      'genex'),
        'guide':      ObjType('guide',      'guide'),
        'variable':   ObjType('variable',   'variable'),
        'module':     ObjType('module',     'module'),
        'policy':     ObjType('policy',     'policy'),
        'prop_cache': ObjType('prop_cache', 'prop_cache'),
        'prop_dir':   ObjType('prop_dir',   'prop_dir'),
        'prop_gbl':   ObjType('prop_gbl',   'prop_gbl'),
        'prop_inst':  ObjType('prop_inst',  'prop_inst'),
        'prop_sf':    ObjType('prop_sf',    'prop_sf'),
        'prop_test':  ObjType('prop_test',  'prop_test'),
        'prop_tgt':   ObjType('prop_tgt',   'prop_tgt'),
        'manual':     ObjType('manual',     'manual'),
    }
    directives = {
        'command':    CMakeObject,
        'envvar':     CMakeObject,
        'genex':      CMakeObject,
        'variable':   CMakeObject,
        # Other `object_types` cannot be created except by the `CMakeTransform`
    }
    roles = {
        'command':    CMakeXRefRole(fix_parens=True, lowercase=True),
        'cpack_gen':  CMakeXRefRole(),
        'envvar':     CMakeXRefRole(),
        'generator':  CMakeXRefRole(),
        'genex':      CMakeXRefRole(),
        'guide':      CMakeXRefRole(),
        'variable':   CMakeXRefRole(),
        'module':     CMakeXRefRole(),
        'policy':     CMakeXRefRole(),
        'prop_cache': CMakeXRefRole(),
        'prop_dir':   CMakeXRefRole(),
        'prop_gbl':   CMakeXRefRole(),
        'prop_inst':  CMakeXRefRole(),
        'prop_sf':    CMakeXRefRole(),
        'prop_test':  CMakeXRefRole(),
        'prop_tgt':   CMakeXRefRole(),
        'manual':     CMakeXRefRole(),
    }
    initial_data = {
        'objects': {},  # fullname -> docname, objtype
    }

    def clear_doc(self, docname):
        to_clear = set()
        for fullname, (fn, _) in self.data['objects'].items():  # NOQA disable=invalid-name
            if fn == docname:
                to_clear.add(fullname)
        for fullname in to_clear:
            del self.data['objects'][fullname]

    def resolve_xref(                                       # NOQA disable=too-many-arguments
        self, env, fromdocname, builder, typ, target, _node, contnode
      ):
        targetid = '{0}:{1}'.format(typ, target)
        obj = self.data['objects'].get(targetid)
        if obj is None:
            # NOTE Sphinx will report not found refs
            return None
        return make_refnode(builder, fromdocname, obj[0], targetid, contnode, target)

    def get_objects(self):
        for refname, (docname, obj_type) in self.data['objects'].items():
            yield (refname, refname, obj_type, docname, refname, 1)

    def resolve_any_xref(                                   # NOQA disable=too-many-arguments
        self, _env, _fromdocname, _builder, _target, _node, _contnode
      ):
        raise NotImplementedError

    def merge_domaindata(self, _docnames, _otherdata):
        raise NotImplementedError


def setup(app):
    app.add_directive('cmake-module', CMakeModule)
    app.add_transform(CMakeTransform)
    app.add_transform(CMakeXRefTransform)
    app.add_domain(CMakeDomain)
    return {'parallel_read_safe': True}
