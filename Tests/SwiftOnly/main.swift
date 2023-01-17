dump("SwiftOnly")

#if SWIFTONLY
dump("SWIFTONLY defined")
#else
fatalError("SWIFTONLY NOT defined")
#endif

#if swift(>=5.2)
@_alwaysEmitIntoClient
public func thirtyTwo() -> Int {
    return 32
}
#endif
