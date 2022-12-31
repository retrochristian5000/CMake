import SwiftOnly

#if swift(>=5.2)
public let ThirtyTwo: Int = SwiftOnly.thirtyTwo()
#else
public let ThirtyTwo: Int = 32
#endif
