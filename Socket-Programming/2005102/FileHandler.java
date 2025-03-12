import java.io.*;
import java.nio.file.Files;

public class FileHandler {
    public void download(File checkForFile,OutputStream outputStream)
    {
        StringBuilder responseHeaders = new StringBuilder();
        responseHeaders.append("HTTP/1.1 200 OK\r\n").append("Accept-Ranges: bytes\r\n");
        responseHeaders.append("Content-Length: ").append(checkForFile.length()).append("\r\n");
        responseHeaders.append("Content-Type: application/octet-stream\r\n").append("Content-Disposition: attachment; filename=\"").append(checkForFile.getName()).append("\"\r\n");
        responseHeaders.append("\r\n");

        // Write the entire response header at once
        try {
            outputStream.write(responseHeaders.toString().getBytes());
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        try(FileInputStream fis=new FileInputStream(checkForFile))
        {
            byte[] buffer=new byte[2048];
            int bytesRead;
            int totalBytesRead=0;

            while((bytesRead=fis.read(buffer))!=-1)
            {
                outputStream.write(buffer,0,bytesRead);
                totalBytesRead+= bytesRead;
            }

            System.out.println("Total bytes sent: "+totalBytesRead);

        }
        catch (IOException e) {
            System.out.println("Download aborts");
        }
    }
    public boolean isImage(File file) throws Exception {
        String determine_MIME=Files.probeContentType(file.toPath());
        if(determine_MIME!=null&&determine_MIME.startsWith("image"))
            return true;  // It is an image

        //check file extension if MIME type is not detected
        String[] imageExtensions={".png",".jpg",".jpeg",".gif",".bmp",".svg",".webp"};
        String fileName=file.getName().toLowerCase();

        for(String extension:imageExtensions)
        {
            if(fileName.endsWith(extension))
            {
                return true;
            }
        }
        return false;
    }
}
