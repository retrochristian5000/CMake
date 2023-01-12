dump("SwiftOnly")

#if SWIFTONLY
dump("SWIFTONLY defined")
#else
fatalError("SWIFTONLY NOT defined")
#endif

@_alwaysEmitIntoClient
public func thirtyTwo() -> Int {
    return 32
}
