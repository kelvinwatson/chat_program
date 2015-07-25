/* Programmed by: Kelvin Watson
 * OSU ID: 932540242
 * ONID: watsokel
 * FileName: chatclient.java
 * Description: Chat client for 2-way communication with a TCP chat server
 * Sources: https://systembash.com/a-simple-java-tcp-server-and-tcp-client/
 * http://docs.oracle.com/javase/tutorial/networking/sockets/readingWriting.html
 */

import java.io.*; 
import java.net.*;
import java.util.Scanner; 

public class chatclient{
  public static void main(String argv[]) throws Exception  {   
    TCPClient client = new TCPClient();
    client.hostName = argv[0];
    client.portNumber = Integer.parseInt(argv[1]);
    try{
      client.initiateContact();          
      client.printTitleAndPrompt();
      
      Scanner input = new Scanner(System.in);
      client.user = input.next();
      if(client.user.length() > 10){
        client.user = client.user.substring(0,10);
      }
      client.handle = client.user + "> ";

      System.out.println("Welcome " + client.user + "! You may now begin chatting.\n");
      char endTransmission = 3;
      /*Chat until command to quit*/
      do{
        String sendAction = client.sendMessage();
        if(sendAction.equals("OK")){
          String receiveAction = client.receiveMessage();
          if(receiveAction.equals("SERVERQUIT")){
            return;
          } 
        }
        else if(sendAction.equals("INTERRUPT") || sendAction.equals("QUIT")){
          break;  
        }
      }while(true);
    } finally {
      System.out.print("\nThank you for using Simple Chat.\n\n");
      System.exit(0);
    }
  } 
}
