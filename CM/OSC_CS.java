package controlmodule.motioncomposer;

import java.awt.Desktop;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PipedOutputStream;
import java.io.PrintStream;
//import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
//import java.net.SocketException;
import java.nio.file.Paths;
import java.util.ArrayList;
//import java.util.Date;
import java.util.HashMap;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JTextArea;

import osc.netutil.*;


public class OSC_CS {
	
	OSC_CS(){
		
	}
	static List<String> csList = new ArrayList<String>();
	static List<String> oscMessageList = new ArrayList<String>(); 
	static String strFileName = "includes/config_.txt";
	static String strProcess="";
	static String strOldValue;
	static String strNewValue;
	static String[] strKeyValuePairs = null;
	static String strLine = null;
	static InetAddress senderIntAdd=null;
	static InetAddress receiverIntAdd=null;
	static int intSenderPort=0;
	static int inReceiverPort = 0;
	static String strReceiverName = null;
	static OSCClient senderTm;
	static OSCClient senderMe;
	static OSCServer receiverTm;
	static OSCServer receiverMe;
	static String clientName=null ;
	OSCMessage clientMsg;
	OSCMessage serverMsg = null;
	static JFrame frame = new JFrame("MotionComposer");
	static JTextArea textField = new JTextArea();
	//static SimpleOSCListener listener = new SimpleOSCListener();
	static String cmPath = Paths.get(".").toAbsolutePath().normalize().toString();
	
	public static void osc() throws IOException, InterruptedException 
	{
		
    	
    	   try {
	            // FileReader reads text files in the default encoding.
	            FileReader fileReader = new FileReader(strFileName);

	            // Always wrap FileReader in BufferedReader.
	            BufferedReader bufferedReader = new BufferedReader(fileReader);

	            while((strLine = bufferedReader.readLine()) != null) {
	            	if(strLine.startsWith("@")) {
	            		strLine = strLine.substring(1);
	            	 	//System.out.println(line);	
	            		continue;
	               	}
	            	if(strLine.startsWith("#")) {
	            	    continue;	
	            	}
	            	if(strLine.contentEquals("CLIENT_SERVER_LIST")) 
	            	{
	            		strProcess= "CLIENT_SERVER_LIST";
	            		//System.out.println(process);
	            		continue;
	            	}
	                else if(strLine.contentEquals("OSC_MESSAGES_LIST"))
	                {
	                	strProcess= "OSC_MESSAGES_LIST";
	                	//System.out.println(process);
	                	continue;
						}
	            	
	            	if(strProcess== "CLIENT_SERVER_LIST") {
	            		
	            		csList.add(strLine);
	            		
									
	            	}
	            	else if (strProcess=="OSC_MESSAGES_LIST") 
	            	{
	            		oscMessageList.add(strLine);
	     
	            	}
	            	
	                }
	       
	            // Always close files.
	            bufferedReader.close(); 
	            //System.out.println(csList);
	        }
	        catch(FileNotFoundException ex) {
	            System.out.println(
	                "Unable to open file '" + 
	                strFileName + "'");                
	        }
	        catch(IOException ex) {
	            System.out.println(
	                "Error reading file '" 
	                + strFileName + "'");                  
	            
	        }
    	
	        	

    	
	
    	   for(String i : csList) 
	{
    		   HashMap<String, String> csMap = new HashMap<String,String>(); 
	        	strOldValue=i;
	        	strNewValue=strOldValue.replaceAll("'", "");
	    		strNewValue=strNewValue.substring(1,strNewValue.length()-1);
	    		strKeyValuePairs=strNewValue.split(",");

	    		for(String pair : strKeyValuePairs)                        
	            {
	                String[] entry = pair.split(":");            
	                csMap.put(entry[0].trim(), entry[1].trim());          

	            }
	    		
	    		String typeValue = csMap.get("type");
	    
	    		if(typeValue.contentEquals("client")) 
	    		{
	    			
	    			String strSenderIp= csMap.get("ip");
	    			senderIntAdd = InetAddress.getByName(strSenderIp);
	    			String strSenderPort = csMap.get("port");
	    			intSenderPort = Integer.parseInt(strSenderPort);
	    			clientName=csMap.get("name");
	    			if(clientName.equals("CMtoTM")) 
	    			{
	    				senderTm = OSCClient.newUsing(OSCClient.UDP);//, new InetSocketAddress( senderIntAdd, intSenderPort));    // Create UDP Clients
	    				//System.out.println(senderTm.getLocalAddress()+ clientName);
	    			}
	    			else
	    			{
	    				senderMe= OSCClient.newUsing(OSCClient.UDP); //, new InetSocketAddress( senderIntAdd, intSenderPort));    // Create UDP Clients
	    				//System.out.println(senderMe.getLocalAddress()+ clientName);
	    			}
	    		}
	    		else if(typeValue.contentEquals("server"))
	    		{
	    			String strReceiverIp= csMap.get("ip");
	    			receiverIntAdd = InetAddress.getByName(strReceiverIp);
	    			String strReceiverPort = csMap.get("port");
	    			inReceiverPort = Integer.parseInt(strReceiverPort);
	    			strReceiverName = csMap.get("name");
	    			System.out.println(strReceiverName);
	    			if(strReceiverName.equals("CMserverTM")) 
	    			{
	    				receiverTm = OSCServer.newUsing(OSCServer.UDP, new InetSocketAddress( receiverIntAdd, inReceiverPort)); // Create UDP Servers
	    				//System.out.println(receiverTm.getLocalAddress()+strReceiverName);
	    				receiverTm.start();
	    				
	    			}
	    			else
	    			{
	    				receiverMe = OSCServer.newUsing(OSCServer.UDP, new InetSocketAddress( receiverIntAdd, inReceiverPort)); // Create UDP Servers
	    				//System.out.println(receiverMe.getLocalAddress()+strReceiverName);
	    				receiverMe.start();
	    			}

	    			
	    		}
	    		
	     	}

    	   receiverTm.addOSCListener(new OSCListener() 
    	   {
    	        public void messageReceived( OSCMessage m, SocketAddress addr, long time )
    	        	{
    	        		String receivedAddress = m.getName();
    	        		String strNa =m.getName();
    	        		consoleMsg(strNa);
    	        		String strAd = addr.toString();
    	        		consoleMsg(strAd);
    	        		
    	        		try {
							msghandler(receivedAddress);
						} 
    	        		catch (IOException e)
    	        		{
    	        			e.printStackTrace();
						} 
    	        		catch (InterruptedException e) 
    	        		{
							e.printStackTrace();
						}
    	        	}
    	   });
    	  
    	   
 
    	   receiverMe.addOSCListener(new OSCListener() 
    	   {
    	        public void messageReceived( OSCMessage m, SocketAddress addr, long time )
    	        	{
    	        	String receivedAddress = m.getName();
    	        	consoleMsg(receivedAddress);
	        		String strrAd= addr.toString();
	        		consoleMsg(strrAd);
	        		
	        		try {
						msghandler(receivedAddress);
					} 
	        		catch (IOException e)
	        		{
	        			e.printStackTrace();
					} 
	        		catch (InterruptedException e) 
	        		{
						e.printStackTrace();
					}
    	        	
    	        	}
    	   });
  
    	   
    	   	
	}

	
	public static void run_osc() throws IOException, InterruptedException 
	{
	
		  	  File fileTm = new File( cmPath+"\\TM\\release\\app\\main");
   	   		  Desktop desktopTm = Desktop.getDesktop();
   	   		  if(fileTm.exists()) desktopTm.open(fileTm);
   	   		  
   	   		  Thread.sleep(10000);	
   	   		  
   	   		  File fileMe = new File( cmPath+"\\MEs\\Tonality\\MEsimulator.maxpat");
	   		  Desktop desktopMe = Desktop.getDesktop();
	   		  if(fileMe.exists()) desktopMe.open(fileMe);
   	   		  
	}

	public static void msghandler(String address) throws IOException, InterruptedException 
	{
		System.out.println(address);
		
		if(address.equals("/set/ME/loaded"))
		
		{
			
			 senderMe.setTarget(new InetSocketAddress(receiverIntAdd,6560));
			 //System.out.println(senderMe.getLocalAddress());
			 senderMe.start();
			 senderMe.send(new OSCMessage( "/set/ME/initialize"));
			 String meInt="/set/ME/initialize";
			 //System.out.println("msg sent");
			 consoleMsg(meInt+"\n");
			 
		}                                     
		
		else if (address.equals("/set/ME/ready")) 
		
		{
			
			
		}
		
		else if  ((address.equals("/set/player/1/soundbank/1/list") )|| (address.equals("/set/player/1/soundbank/2/list"))||(address.equals("/set/player/2/soundbank/1/list")) ||(address.equals("/set/player/2/soundbank/2/list"))) 
		
		{
			
			
		}
		
		else if (address =="/set/xxx") 
		
		{
			
			
		}
		
		else if (address == "/set/xxx") 
		
		{
			
			
		}
			
		
		else if(address.equals("/set/TM/loaded")) 
		
		{
			 
			 
			 senderTm.setTarget(new InetSocketAddress( "127.0.0.1", 6561));
			 senderTm.start();
			 senderTm.send( new OSCMessage( "/set/TM/initialize"));
			 String strTm = "/set/TM/initialize";
			 consoleMsg(strTm+"\n");
			 //System.out.println("msg sent");
		}
	
		else if(address.equals("/set/TM/ready")) 
		
		{
			
			String nwMsg = "msg received";
			consoleMsg(nwMsg+"\n");
			
		}
		
	}
	
	public static void display() 
	{
		ImageIcon img = new ImageIcon("includes/logo1.gif");
		
		  //JFrame frame = new JFrame("MotionComposer");
		 
		  	 frame.setSize(500,200);
		     frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		     frame.setIconImage(img.getImage());
		  
		  
		  	//JTextArea textField = new JTextArea();
	        textField.setBounds(150, 150, 280, 150);
	        textField.setVisible(true);
	        frame.add(textField);
	        frame.setVisible(true);
	        
	        PipedOutputStream pOut = new PipedOutputStream();
	        System.setOut(new PrintStream(pOut,true));
	        
	        
	}
	
	public static void consoleMsg(String msgCon) 
	{
		textField.append(msgCon+"\n");
	}
	
}
