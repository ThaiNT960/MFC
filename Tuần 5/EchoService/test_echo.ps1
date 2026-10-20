$ErrorActionPreference = 'Stop'

$client = [System.Net.Sockets.TcpClient]::new('127.0.0.1', 9999)
$stream = $client.GetStream()
$stream.ReadTimeout = 5000

while ($true) {
    $message = Read-Host 'Nhap tin nhan'
    if ($message -eq 'exit') { break }
    if ($message.Length -eq 0) { continue }

    $data = [System.Text.Encoding]::UTF8.GetBytes($message)
    $stream.Write($data, 0, $data.Length)

    $reply = [byte[]]::new($data.Length)
    $total = 0
    while ($total -lt $reply.Length) {
        $count = $stream.Read($reply, $total, $reply.Length - $total)
        if ($count -eq 0) { throw 'EchoService da dong ket noi.' }
        $total += $count
    }

    Write-Host ('Echo: ' + [System.Text.Encoding]::UTF8.GetString($reply))
}

$stream.Close()
$client.Close()
