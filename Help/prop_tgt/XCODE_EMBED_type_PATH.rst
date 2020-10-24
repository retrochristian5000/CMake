XCODE_EMBED_<type>_PATH
-----------------------

.. versionadded:: 3.20

Tell the :generator:`Xcode` generator the name of the subdirectory where to place
embedded libraries or frameworks in the target bundle.

Path will be treated as relative one.

`<type>` specified which embed build phase to use, currently supported is
`FRAMEWORK` type which will embed file in Framework path.
