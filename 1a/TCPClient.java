/* Programmed by: Kelvin Watson
 * OSU ID: 932540242
 * ONID: watsokel
 * FileName: TCPClient.java
 * Description: Methods required by the class chatclient.java for two-way 
 * communication with server program
 * Sources: https://systembash.com/a-simple-java-tcp-server-and-tcp-client/
 * http://docs.oracle.com/javase/tutorial/networking/sockets/readingWriting.html
 */

//http://docs.oracle.com/javase/tutorial/networking/sockets/readingWriting.html
import java.io.*; 
import java.net.*;
import java.util.Scanner; 

public class TCPClient {  
    public String hostName;
    public int portNumber;
    public String user;
    public String handle;
    public String command;
    public String message;
    public String response;
    public String handleMsg;
    public String lengthStr;
    public BufferedReader inFromUser;
    public DataOutputStream outToServer;
    public BufferedReader inFromServer;
    public Socket clientSocket;

    /*Default constructor
    *Pre-conditions: none
    *Post-conditions: initializes lengthStr to empty string
    */
    public TCPClient(){
  	  lengthStr="";
      inFromUser = null;
      outToServer = null;
      inFromServer = null;
      clientSocket = null;
    }

    /*Method to print title and prompt for user handle
    *Pre-conditions: none
    *Post-conditions: no return value
    */
    public void printTitleAndPrompt(){
        System.out.println("\n\nSimple Chat System (CLIENT)");
        System.out.println("Programmed by Kelvin Watson, OSU ID 932540242, onid: watsokel)");
        System.out.println("**************************************************************");
        System.out.println("TCP Connection established. Welcome to Simple Chat.");
        System.out.print("Enter a user name (max 10 characters): ");
    }

    /*Method for creating client socket and buffers
    *Pre-conditions: member variable must be declared
    *Post-conditions: client socket and buffers allocated
    */
    public void initiateContact(){
      try{
        clientSocket = new Socket(hostName, portNumber);   
        inFromUser = new BufferedReader(new InputStreamReader(System.in));   
        outToServer = new DataOutputStream(clientSocket.getOutputStream());   
        inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream())); 
      }catch(IOException e){
         System.err.println("Caught IOException in initiateContact: " + e.getMessage());
      }
    }

    /*Method for sending messages to the server
    *Pre-conditions: client must have established TCP connection with server
    *Post-conditions: client socket and buffers allocated
    */
    public String sendMessage(){
        System.out.print(handle);
        try{ 
          message = inFromUser.readLine();
          if(message.equals("SIGINT")){
            outToServer.writeBytes("SIGINT");
            System.out.println("\nTCP connection closed and program terminated.");
            response = inFromServer.readLine();
            if(!response.isEmpty()){
              System.out.println(response + " message received from server means OK to close connection");
              clientSocket.close();
            }
            return "INTERRUPT";
          } 
          else if(message.equals("\\quit")){
            outToServer.writeBytes("SIGQUIT");
            System.out.println("\nTCP connection closed.");
            response = inFromServer.readLine();
            if(!response.isEmpty()) {
              System.out.println(response + " message received from server means OK to close connection on this side");
              clientSocket.close();
            }
            return "QUIT";
          }
          else{
            handleMsg = handle + message;
            int lengthInt = handleMsg.length();
              outToServer.writeBytes(handleMsg); 
            return "OK";
          }
        }catch(IOException e){
          System.err.println("Caught IOException in sendMessage(): " + e.getMessage());
        } 
        return null;
    }

    /*Method for receiving messages from the server
    *Pre-conditions: client must have established TCP connection with server
    *Post-conditions: client socket and buffers allocated
    */
    public String receiveMessage(){
      try{
        response = inFromServer.readLine();
      }catch(IOException e){
        System.err.println("Caught IOException in inFromServer.readLine(): " + e.getMessage());
      }
      if(response.equals("\\quit")){
        System.out.println("Server quit command received.");  
        return "SERVERQUIT";
      }
      System.out.println(response);
      return "SENT";
    }
}