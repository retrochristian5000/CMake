# -*- coding: utf-8 -*-

# Third party packages
from pygments.style import Style
from pygments.token import (
    Comment,
    Name,
    Number,
    Operator,
    Punctuation,
    String,
    Whitespace
)


class CMakeTemplateStyle(Style):                            # NOQA disable=too-few-public-methods
    """
        for more token names, see pygments/styles.default
    """

    background_color = '#f8f8f8'
    default_style = ''

    styles = {
        Comment:                   'italic #666666',
        Name.Constant:             '#4070a0',               # UPPER_CASE function args
        Name.Entity:               '#0095ff',               # aliased target name
        Name.Function:             '#007020',
        Name.Label:                '#1080b0',               # <variable-names> (in angle brackets)
        Name.Namespace:            '#0095ff',               # target's namespace `NS::*`
        Name.Tag:                  '#aa50c5',               # `ENV`, `CACHE`, or `DATA`
        Name.Variable:             '#bb60d5',               # variable name inside `${...}`
        Number:                    '#40b0d0',
        Operator:                  '#6cbd1b',               # genex $<...>
        Operator.Word:             '#6cbd1b',               # genex name
        Punctuation:               '#444444',
        String:                    '#cc6699',
        String.Escape:             '#aa55ff',               # escape chars as defined by `cmake-language.7`
        String.Heredoc:            '#904466',               # bracket strings
        String.Interpol:           '#bb60d5',               # ${...}
        String.Other:              '#555555',               # unquoted params
        String.Symbol:             '#aa55ff',               # e.g., line continuation
        Whitespace:                '#bbbbbb',
    }
