:orphan:

.. _WINDOWS_REGISTRY_QUERY:

Windows Registry Query Syntax
-----------------------------

The formal syntax, as specified using
`BNF <https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form>`_ notation with
the regular extensions, for registry query is the following:

.. raw:: latex

   \begin{small}

.. productionlist::
  registry_query: '[' `sep_definition`? `root_key`
                :     ((`key_separator` `sub_key`)? (`value_separator` `value_name`_)?)? ']'
  sep_definition: '{' `value_separator` '}'
  root_key: 'HKLM' | 'HKEY_LOCAL_MACHINE' | 'HKCU' | 'HKEY_CURRENT_USER' |
          : 'HKCR' | 'HKEY_CLASSES_ROOT' | 'HKCC' | 'HKEY_CURRENT_CONFIG' |
          : 'HKU' | 'HKEY_USERS'
  sub_key: `element` (`key_separator` `element`)*
  key_separator: '/' | '\\'
  value_separator: `element` | ';'
  value_name: `element` | '(default)'
  element: `character`\+
  character: <any character except `key_separator` and `value_separator`>

.. raw:: latex

   \end{small}

The :token:`sep_definition` optional item offers the possibility to specify
the string used to separate the :token:`sub_key` from the :token:`value_name`
item. If not specified, the character ``;`` is used. Multiple
:token:`registry_query` items can be specified as part of a path.

.. code-block:: cmake

  # example using default separator
  set (REG_QUERY1 "/root/[HKLM/Stuff;InstallDir]/lib[HKLM\\\\Stuff;Architecture]")

  # example using different specified separators
  set (REG_QUERY2 "/root/[{|}HKCU/Stuff|InstallDir]/lib[{@@}HKCU\\\\Stuff@@Architecture]")

If the :token:`value_name` item is not specified or has the special name
``(default)``, the content of the default value, if any, will be returned. The
supported types for the :token:`value_name` are:

* ``REG_SZ``.
* ``REG_EXPAND_SZ``. The returned data is expanded.
* ``REG_DWORD``.
* ``REG_QWORD``.

When the registry query failed, typically because the key does not exist or
the data type is not supported, the string ``/REGISTRY-NOTFOUND`` is substituted
to the ``[]`` query expression.
