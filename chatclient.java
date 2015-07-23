/* Programmed by: Kelvin Watson
 * OSU ID: 932540242
 * ONID: watsokel
 * FileName: chatclient.java
 * Description: Chat client for 2-way communication with server program
 * Sources: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 */

//source: https://systembash.com/a-simple-java-tcp-server-and-tcp-client/
//http://docs.oracle.com/javase/tutorial/networking/sockets/readingWriting.html
import java.io.*; 
import java.net.*;
import java.util.Scanner; 

class chatclient {  
  public static void main(String argv[]) throws Exception  {   
    String hostName = argv[0];
    int portNumber = Integer.parseInt(argv[1]);
    String user, handle, command, message, response, handleMsg, lengthStr="";   
    /*Code to get username up to 10chars*/
    
    BufferedReader inFromUser = null;
    DataOutputStream outToServer = null;
    BufferedReader inFromServer = null;

    Socket clientSocket = null;

    try{
      clientSocket = new Socket(hostName, portNumber);   
      inFromUser = new BufferedReader(new InputStreamReader(System.in));   
      outToServer = new DataOutputStream(clientSocket.getOutputStream());   
      inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));   
            
      System.out.println("\n\nSimple Chat System (CHATCLIENT)");
      System.out.println("Programmed by Kelvin Watson, OSU ID 932540242, onid: watsokel)");
      System.out.println("**************************************************************");
      System.out.println("TCP Connection established. Welcome to Simple Chat.");
      System.out.print("Enter a user name (max 10 characters): ");
      Scanner input = new Scanner(System.in);
      user = input.next();
      if(user.length() > 10){
        user = user.substring(0,10);
      }
      handle = user + "> ";
      System.out.println("Welcome " + user + "! You may now begin chatting.\n");
      /*send it out with the message*/
      char endTransmission = 3;
      /*Chat until command to quit*/
      do{
        System.out.print(handle); 
        message = inFromUser.readLine();   
        if((!message.equals("\\quit")) && (!message.equals("SIGINT"))){
          handleMsg = handle + message;
          int lengthInt = handleMsg.length();
          outToServer.writeBytes(handleMsg); 
          response = inFromServer.readLine();
          System.out.println(response);   
        }
        else if(message.equals("\\quit")){
          outToServer.writeBytes("SIGQUIT");
          System.out.println("\nTCP connection closed.");
          response = inFromServer.readLine();
          if(!response.isEmpty()) {
            System.out.println(response + " message received from server means OK to close connection on this side");
            clientSocket.close();
          }
          break;
        }
        else if(message.equals("SIGINT")){
          outToServer.writeBytes("SIGINT");
          System.out.println("\nTCP connection closed and program terminated.");
          response = inFromServer.readLine();
          if(!response.isEmpty()){
            System.out.println(response + " message received from server means OK to close connection on this side");
            clientSocket.close();
          }
          break;  
        }
      }while(true);

    } finally {
      System.out.print("\nThank you for using Simple Chat.\n\n");
      //clientSocket.close();

    }
  } 
}
