import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class HTTPServer
{
    static final int PORT=5102;
    public static String fileName="2005102_logfile.txt";
    public static PrintWriter logFile;

    public static void main(String[] args) throws IOException {
        ServerSocket serverConnect = new ServerSocket(PORT);
        System.out.println("Server started.\nListening for connections on port : " + PORT + " ...\n");
        logFile=new PrintWriter(new FileOutputStream(fileName));
        while(true)
        {
            Socket s=serverConnect.accept();//waits for a client to connect to the server
            System.out.println("Successfully established connection with server");
            new Thread(new HTTPServerThread(s)).start();//responsible for handling the client's request
        }
    }
}
