set(EXTERNAL_APPEXT stickers)

add_library(${EXTERNAL_APPEXT} MODULE Info.plist)

set_target_properties(${EXTERNAL_APPEXT}
  PROPERTIES
  LINKER_LANGUAGE CXX
  BUNDLE YES
  XCODE_PRODUCT_TYPE "com.apple.product-type.app-extension.messages-sticker-pack"
  XCODE_EXPLICIT_FILE_TYPE "wrapper.app-extension"
)
