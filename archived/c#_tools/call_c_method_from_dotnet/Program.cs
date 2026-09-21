using System;
using System.Runtime.InteropServices;

class Program
{
  /*
   gcc -shared -o libmyclib.so myclib.c
   cp libmyclib.so /home/leo_zhang/Documents/GitHub/test/test/bin/Debug/net8.0/
   dotnet run
  */
    // Import the C function from the DLL
    // For Linux/macOS, use [DllImport("libmyclib.so")]
    [DllImport("libmyclib.so")]
    public static extern void my_function();

    static void Main()
    {
        // Call the C function
        my_function();
    }
}

