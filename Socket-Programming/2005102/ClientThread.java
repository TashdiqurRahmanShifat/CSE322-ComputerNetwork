import java.io.*;
import java.net.Socket;
import java.nio.file.Paths;

public class ClientThread implements Runnable
{

    private String fileName;
    FileHandler fileHandler=new FileHandler();
    public ClientThread(String fileName)
    {
        this.fileName=fileName;
    }

    private void handleUpload(Socket socket,File file)throws IOException
    {
        String takeResponse=new BufferedReader(new InputStreamReader(socket.getInputStream())).readLine();
        if(takeResponse!=null&&takeResponse.startsWith("FILE"))
        {
                DataOutputStream dataOutputStream=new DataOutputStream(socket.getOutputStream());
                FileInputStream fileInputStream=new FileInputStream(file);
                dataOutputStream.writeLong(file.length());
                byte[] buffer=new byte[2048];
                int byteRead;
                while((byteRead=fileInputStream.read(buffer))!=-1)
                {
                    dataOutputStream.write(buffer,0,byteRead);
                    dataOutputStream.flush();
                }
                System.out.println(file.getName()+" is uploaded");
                fileInputStream.close();

            }


    }
    private String prepareUpload(File file,String fileName)
    {
        if(!file.exists())return "NonExistent"+fileName;
        boolean isImg= false;
        try {
            isImg = fileHandler.isImage(file);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
        boolean text=fileName.endsWith(".txt")||fileName.endsWith(".mp4");
        if(!isImg&&!text)
        {
            return "Unsupported"+fileName;
        }
        return "Uploaded"+fileName;
    }

    @Override
    public void run() {
        try
        {
            Socket socket=new Socket("localhost",5102);
            //Fetching the filePath
            File generateFile=new File(Paths.get(Paths.get("").toAbsolutePath().toString(),"UploadContent",fileName).toString());
            System.out.println(Paths.get(Paths.get("").toAbsolutePath().toString(),fileName).toString());
            this.fileName=generateFile.getName();

            OutputStreamWriter outputStreamWriter=new OutputStreamWriter(socket.getOutputStream());
            BufferedWriter bufferedWriter=new BufferedWriter(outputStreamWriter);

            String uploadReqFile=prepareUpload(generateFile,fileName);
            bufferedWriter.write(uploadReqFile+'\n');
            bufferedWriter.flush();

            if(uploadReqFile.startsWith("Uploaded"))
            {
                handleUpload(socket,generateFile);
            }
            else
            {
                System.out.println(fileName+" is invalid file");
            }
            System.out.println("Enter Your Filename to Upload.Enter q to Exit");
            socket.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}



