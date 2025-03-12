import java.util.Scanner;

public class Client
{
    private void uploadFiles(String[] upload)
    {
        for(String file:upload) {
            if (file.isEmpty()) continue;
            try {
                Thread uploadThread = new Thread(new ClientThread(file));
                uploadThread.start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    public void takeInput()
    {
        try
        {
            Scanner scn=new Scanner(System.in);
            System.out.println("Enter Your Filename to Upload.Enter q to Exit");
            while (true)
            {
                String userInput=scn.nextLine().trim();
                if(userInput!=null)
                {
                    if(userInput.equalsIgnoreCase("q"))
                    {
                        System.out.println("Terminating...");
                        break;
                    }
                    else
                    {
                        String[] filesForUpload=userInput.split(" ");
                        uploadFiles(filesForUpload);
                    }

                }

            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
    public static void main(String[] args) {
        Client client=new Client();
        client.takeInput();
    }
}
