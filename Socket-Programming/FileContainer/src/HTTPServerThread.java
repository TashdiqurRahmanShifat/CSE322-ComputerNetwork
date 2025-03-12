import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Base64;
import java.util.Date;

public class HTTPServerThread implements Runnable
{
    private final Socket socket;

    private static final String errorFileContent="<html><body><h1>404:Page Not Found</h1></body></html>";
    public String imageViewerContent="<html><body><h4>{title}</h4><img src=\"{src}\"></body></html>";
    private static final String textFileContent="<html><body><h4>{title}</h4><pre>{content}</pre></body></html>";
    private static final String directoryViewerContent="<html><body><h4>{title}</h4><ul>{items}</ul></body></html>";
    FileHandler fileHandler=new FileHandler();
    BufferedReader reader=null;
    HTTPServerThread(Socket socket)throws IOException
    {
        this.socket=socket;
    }

    private boolean isErr(String fileType)
    {
        if(fileType.equals(errorFileContent)) {
            System.out.println("404:Page Not Found");
            return true;
        }
        return false;
    }



    @Override
    public void run() {
        try
        {
            InputStreamReader inputStreamReader=new InputStreamReader(socket.getInputStream());
            reader=new BufferedReader(inputStreamReader);
            String getHTTP=reader.readLine();

            if(getHTTP==null)return;

            PrintWriter printWriter=new PrintWriter(socket.getOutputStream());

            OutputStream outputStream=socket.getOutputStream();

            String fileType=errorFileContent;

            if(getHTTP.length()>0)
            {
                if(getHTTP.startsWith("GET"))
                {
                    String[] parts=getHTTP.split(" ");
                    String route=parts[1];
                    //Building Full Path

                    String fullpath=Paths.get(Paths.get("").toAbsolutePath().toString(),route).toString();
                    System.out.println(fullpath);

                    File checkForFile=new File(fullpath);
                    if(checkForFile.exists())
                    {
                        if(checkForFile.isDirectory())
                        {
                            StringBuilder listItemBuilder=new StringBuilder();
                            File[] files=checkForFile.listFiles();
                            if(files!=null)
                            {
                                for (File f:files)
                                {
                                    String fileName=f.getName();
                                    String fileHref=(route+"/"+fileName).replaceAll("//","/");
                                    String listItem=f.isDirectory()?"<li><a href=\""+fileHref+"\"><b><i>"+fileName+"</i></b></a></li>":
                                            "<li><a href=\""+fileHref+"\">"+fileName+"</a></li>";
                                    listItemBuilder.append(listItem).append("\n");
                                }

                                fileType=this.directoryViewerContent.replace("{title}",checkForFile.getName())
                                        .replace("{items}",listItemBuilder.toString());
                            }
                        }

                        //May be file or anything
                        else
                        {
                            //for File
                            if(checkForFile.getName().endsWith(".txt"))
                            {
                                String fileContent=new String(Files.readAllBytes(checkForFile.toPath()),StandardCharsets.UTF_8);
                                fileType=textFileContent.replace("{title}",checkForFile.getName()).replace("{content}",fileContent);
                            }
                            else if(fileHandler.isImage(checkForFile))
                            {
                                byte[] imgByte=Files.readAllBytes(checkForFile.toPath());
                                String base64Img= Base64.getEncoder().encodeToString(imgByte);
                                String mimeType=Files.probeContentType(checkForFile.toPath());
                                if(mimeType==null)
                                {
                                    throw new IOException("Could not determine MIME type for the image.");
                                }
                                String imgSrc="data:"+mimeType+";base64,"+base64Img;
                                fileType=imageViewerContent.replace("{title}",checkForFile.getName()).replace("{src}",imgSrc);
                            }
                            else
                            {
                                fileHandler.download(checkForFile,outputStream);
                            }
                        }
                    }
                    if(fileType!=null)
                    {
                        String request=getHTTP+"\nDate: "+new Date()+"\rContent-Type: text/html\r\nContent-Length: "+fileType.length()+"\r\n\r\n";

                        HTTPServer.logFile.println("REQUEST\n"+request);

                        String st=isErr(fileType)?"404: Page Not Found":"200 OK";
                        String response="HTTP/1.0 "+st+"\r\n"+"Server: Java HTTP Server: 1.0\r\nDate: "+new Date()+"\r\n"+
                                "Content-Type: text/html"+"\r\nContent-Length: "+fileType.length()+
                                "\r\n\r\n";
                        HTTPServer.logFile.println("RESPONSE\n"+response);
                        HTTPServer.logFile.flush();
                        response+=fileType;
                        printWriter.write(response);//Sending response to the server
                    }
                    printWriter.flush();
                    printWriter.close();
                    outputStream.flush();
                    outputStream.close();
                }
                else
                {
                    if(getHTTP.startsWith("Uploaded"))
                    {
                        DataInputStream dataInputStream = new DataInputStream(socket.getInputStream());
                        BufferedWriter bufferedWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
                        bufferedWriter.write("FILE_READY_TO_UPLOAD" + "\n");
                        bufferedWriter.flush();

                        String fileName= getHTTP.substring("Uploaded".length());
                        System.out.println(fileName+" is uploaded");

                        while (dataInputStream.available()>0)
                            dataInputStream.read();

                        File uploadDirectory=new File("uploaded");
                        if(!uploadDirectory.exists()){
                            uploadDirectory.mkdir();
                        }

                        System.out.println();
                        //Path filePath=Paths.get(Paths.get(Paths.get("").toAbsolutePath().toString(),"RootContent").toString(),"uploaded",fileName);
                        Path filePath=Paths.get(Paths.get("").toAbsolutePath().toString(),"uploaded",fileName);
                        try(FileOutputStream fileOutputStream=new FileOutputStream((filePath.toString())))
                        {
                            long fileSize= dataInputStream.readLong();
                            byte[] buffer=new byte[2048];
                            int byteRead;
                            while(fileSize>0)
                            {
                                // Read a chunk of data from the input stream
                                byteRead=dataInputStream.read(buffer,0,(int)Math.min(buffer.length,fileSize));
                                if(byteRead==-1)break;//End of stream
                                fileOutputStream.write(buffer,0,byteRead);
                                fileSize-=byteRead;
                            }
                            //fileOutputStream.close();
                        }
                        catch (Exception e)
                        {
                            e.printStackTrace();
                        }
                    }
                    else if(getHTTP.startsWith("NonExistent") || getHTTP.startsWith("Unsupported"))
                    {
                        System.out.println(getHTTP + " is invalid request");
                    }
                }
            }

            this.socket.close();
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
}
