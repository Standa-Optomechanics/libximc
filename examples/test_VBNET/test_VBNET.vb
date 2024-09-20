Imports System
Imports System.Text
Imports System.Threading
Imports ximc

Namespace test_VBNET
    Class testapp
        Private Shared Sub print_status(ByVal status As status_t)
            Console.WriteLine("rpm: {0} pos: {1} flags: {2}", _
                status.CurSpeed, status.CurPosition, _
             status.flags)
        End Sub

        Public Shared Sub Main(ByVal args As String())
            Dim device As Integer = -1
            Dim res As Result
            Dim probe_flags As Integer = Flags.ENUMERATE_PROBE + Flags.ENUMERATE_NETWORK
            Dim enum_hints As String = "addr=192.168.1.1,172.16.2.3"
            Try
                Dim device_enumeration As IntPtr
                Dim device_count As Integer

                device_enumeration = API.enumerate_devices(probe_flags, enum_hints)
                If device_enumeration = IntPtr.Zero Then
                    Throw New Exception("Error " & res.ToString())
                End If

                device_count = API.get_device_count(device_enumeration)
                If device_count = 0 Then
                    Throw New Exception("No devices")
                End If

                For i As Integer = 0 To device_count - 1
                    Dim dev As String
                    dev = API.get_device_name(device_enumeration, i)
                    System.Console.WriteLine("Found device {0}", dev)
                Next

                Dim deviceName As [String] = API.get_device_name(device_enumeration, 0)
                System.Console.WriteLine("Using device {0}", deviceName)

                device = API.open_device(deviceName)
                Console.WriteLine("Device {0}", device)

                Dim status As status_t
                res = API.get_status(device, status)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If
                print_status(status)

                Dim status_calb As status_calb_t
                Dim calibration As calibration_t
                calibration.A = 0.1
                calibration.MicrostepMode = 9
                res = API.get_status_calb(device, status_calb, calibration)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If
                'print_status(status)

                res = API.command_zero(device)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If

                res = API.get_status(device, status)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If
                print_status(status)

                Console.WriteLine("running...")
                res = API.command_right(device)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If

                Thread.Sleep(5 * 1000)

                res = API.get_status(device, status)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If
                print_status(status)
                Dim shift As Integer
                shift = status.CurPosition

                Console.WriteLine("moving and waiting for stop...")
                res = API.command_move(device, 0, 0)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If


                res = API.command_wait_for_stop(device, 100)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If

                res = API.get_status(device, status)
                If res <> Result.ok Then
                    Throw New Exception("Error " & res.ToString())
                End If
                print_status(status)

                Console.WriteLine("Done...")
            Catch e As Exception
                Console.WriteLine("Exception " & e.Message)
            Finally
                If device <> -1 Then
                    API.close_device(device)
                End If
            End Try
            Console.WriteLine("Press Enter to exit...")
            Console.ReadLine()
        End Sub
    End Class
End Namespace

