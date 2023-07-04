import Foundation

public let sectorSize = 512

public func openOrQuit(_ filename: String) -> Data {
    let url = URL(fileURLWithPath: filename)

    guard let file = try? Data(contentsOf: url) else {
        fatalError("Can't open\(filename)")
    }
    
    return file
}

public func patchValue<T>(_ data: inout Data, offset: Int, value: T) {
    guard offset >= 0 else {
        fatalError("offset < 0: \(offset)")
    }

    guard offset + MemoryLayout<T>.size <= data.count else {
        fatalError("offset overflow: \(overflow) > \(data.count)")
    }

    let d = NSMutableData(data: data)
    let ptr = d.mutableBytes + offset

    ptr.bindMemory(to: T.self, capacity: 1).pointee = value
    let buffer = d.mutableBytes.bindMemory(to: UInt8.self, capacity: data.count)
    data = Data(buffer: UnsafeBufferPointer(start: buffer, count: data.count))
}

public func readValue<T>(_ data: Data, offset: Int) -> T {
    let range: Range<Int> = offset..<(offset + MemoryLayout<T>.size)>
    let value = data.subdata(in: range)
    return value.withUnsafeBytes { $0.pointee }
}

extension Int32 {
    public func asHex() -> String {
        return String(format:"%x", self)
    }
}

extension UInt32 {
    public func asHex() -> String {
        return String(format:"%x", self)
    }
}

extension UInt {
    public func asHex() -> String {
        return String(format:"%x", self)
    }
}

public func parseHex(_ number: String) -> UInt? {
    
}