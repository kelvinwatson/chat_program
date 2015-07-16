//source: https://systembash.com/a-simple-java-tcp-server-and-tcp-client/
//http://docs.oracle.com/javase/tutorial/networking/sockets/readingWriting.html
import java.io.*; 
import java.net.*;
import java.util.Scanner; 

class TCPClient {  
	public static void main(String argv[]) throws Exception  {   
		String hostName = argv[0];
		int portNumber = Integer.parseInt(argv[1]);

		String user, handle, command, message, response;   
		/*Code to get username up to 10chars*/
		
		BufferedReader inFromUser = null;
		DataOutputStream outToServer = null;
		BufferedReader inFromServer = null;

		Socket clientSocket = null;

		try{
			inFromUser = new BufferedReader(new InputStreamReader(System.in));   
			clientSocket = new Socket(hostName, 30020);   
			outToServer = new DataOutputStream(clientSocket.getOutputStream());   
			inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));   

						
			System.out.println("\n\nConnection successful. Welcome to Simple Chat.");
			System.out.println("**********************************************");
			System.out.print("Enter a user name (max 10 characters): ");
			Scanner input = new Scanner(System.in);
			user = input.next();
			//HOW TO LIMIT TO 10 CHARS?
			handle = user + "> ";
			System.out.println("Welcome " + user + "! You may now begin chatting.\n");
			/*send it out with the message*/

			/*Chat until command to quit*/
			do{
				System.out.print(handle);	
				message = inFromUser.readLine();   
				if(!message.equals("\\quit")){
					outToServer.writeBytes(message + '\n');   
					response = inFromServer.readLine();   
					System.out.println("FROM SERVER: " + response);   
				}
				else{
					System.out.println("Chat terminated. ");
					break;
				}
			}while(message != "\\quit");

		} finally {
			if(inFromServer!= null) inFromServer.close();
			if(outToServer!= null) outToServer.close();
			clientSocket.close();
			System.out.print("Connection terminated");
		}
	} 
}
