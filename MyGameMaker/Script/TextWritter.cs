//using System;
//using System.IO;
//using System.Text;


//namespace Pu
//{
//    //public class ConsoleRedirect
//    //{
//    //    // C++ callback to handle the output
//    //    public static Action<string> LogCallback;

//    //    // This method sets the custom writer to handle the console output
//    //    public static void Initialize()
//    //    {
//    //        Console.SetOut(new TextWriterRedirect(LogCallback));
//    //    }

//    //    public static void Main()
//    //    {
//    //        // Initialize the console redirection to C++
//    //        ConsoleRedirect.Initialize();

//    //        // Your C# script logic here
//    //        Console.WriteLine("Hello from C#!");
//    //    }
//    //}

//    public class TextWriterRedirect/* : TextWriter*/
//    {


//        [MonoPInvokeCallback(typeof(Action))]
//        public static extern void MyNativeMethod();

//        //private Action<string> _logCallback;

//        //public TextWriterRedirect(Action<string> logCallback)
//        //{
//        //    _logCallback = logCallback;
//        //}

//        //public override Encoding Encoding => Encoding.UTF8;

//        //// Redirect output to C++ callback
//        //public override void WriteLine(string value)
//        //{
//        //    _logCallback?.Invoke(value); // Send the output to C++ log
//        //}
//    }
//}