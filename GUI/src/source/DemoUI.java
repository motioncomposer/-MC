package source;


//import moco.Main;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.UnknownHostException;
import java.time.Duration;
import osc.netutil.*;
import osc.netutil.test.*;

import com.sun.javafx.css.Style;

///import controlmodule.motioncomposer.OSC_CS;
//import osc.netutil.*;
//import osc.netutil.test.*;
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.geometry.VPos;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonBar.ButtonData;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Hyperlink;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.RadioButton;
import javafx.scene.control.Slider;
import javafx.scene.control.TextArea;
//import javafx.scene.control.Label;
//import javafx.scene.control.PasswordField;
import javafx.scene.control.TextField;
import javafx.scene.control.ToggleGroup;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.FlowPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Pane;
import javafx.scene.layout.Priority;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.TilePane;
import javafx.scene.layout.VBox;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;
import javafx.scene.media.MediaView;
import javafx.scene.paint.Color;
import javafx.scene.paint.CycleMethod;
import javafx.scene.paint.LinearGradient;
import javafx.scene.paint.Stop;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.stage.Stage;

 public class DemoUI extends Application {
	static String strrAd;
	static VBox vbox;
	static VBox vbox3;
	static TextArea messageArea;
	static OSCClient guiclient;
	static InetSocketAddress GUItoCM = null;
	private static String host="127.0.0.1";
	InetAddress senderIntAdd = null;
	 
	static int intSenderPort=6265;
    private static final String MEDIA_URL =
"http://download.oracle.com/otndocs/products/javafx/oow2010-2.flv";
    
    @Override
 
        public void start(Stage primaryStage) throws Exception
  
        {               
    	
            StackPane root = new StackPane();
  
            HBox hbox = new HBox(30); // create a HBox to hold 2 vboxes       
  
            // create a vbox with a textarea that grows vertically

            vbox = new VBox(10);       
  
            Label lbName = new Label("Volume");

            Slider slideV = new Slider(0,100,50);
            slideV.setOrientation(Orientation.VERTICAL); 
            
            Label lbNameS = new Label("Sensitivity");
            Slider slideS = new Slider(0,100,50);
            slideS.setOrientation(Orientation.VERTICAL);
  
            Button startbtn = new Button("Start");
            Button stopbtn =  new Button("Stop");
            vbox.getChildren().addAll(lbName,slideV,lbNameS,slideS,startbtn,stopbtn);
           
            vbox.setVisible(false);
   
            // create a vbox that grows horizontally inside the hbox
  
            VBox vbox2 = new VBox(10);       
    
            Label lbName2 = new Label("Video Output");
   
            Media media = new Media(MEDIA_URL); 
            MediaPlayer mediaPlayer = new MediaPlayer(media);
            mediaPlayer.setAutoPlay(true);

            // Create the view and add it to the Scene.
            MediaView mediaView = new MediaView(mediaPlayer);
            
            Label lbName5 = new Label("Communication Msgs");
            
            messageArea = new TextArea();
            
            //messageArea.setText(strrAd);
            
            HBox btn = new HBox(10);
            
            Button Op1 = new Button("Tonality");	
            Op1.setOnAction(new EventHandler<ActionEvent>() {
			@Override
            public void handle(ActionEvent event) {
            	vbox.setVisible(true);
            	vbox3.setVisible(true);
            	messageArea.appendText("Tonality is Selected\n");
            	
            	try {
					guiclient = OSCClient.newUsing(OSCClient.UDP);
				} catch (IOException e3) {
					// TODO Auto-generated catch block
					e3.printStackTrace();
				}
				try {
					senderIntAdd = InetAddress.getByName(host);
				} catch (UnknownHostException e2) {
					// TODO Auto-generated catch block
					e2.printStackTrace();
				}
				GUItoCM = new InetSocketAddress(senderIntAdd, intSenderPort);
            	guiclient.setTarget(GUItoCM);
            	try {
					guiclient.start();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
            	try {
					guiclient.send(new OSCMessage( "start"));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
            }
           //vbox.setVisible(true);
            });
            
            Button Op2 = new Button("Drums");
            
            Button Op3 = new Button("Piano");
            
            btn.getChildren().addAll(Op1,Op2,Op3);
         
            vbox2.getChildren().addAll(lbName2, mediaView, btn,lbName5,messageArea);
   
               

           
  
//            StackPane.setMargin(hbox, new Insets(20));
            
            vbox3 = new VBox(10);
            
            Label lbName3 = new Label("Player Control");
            
            RadioButton btn1 = new RadioButton("one player");
            
            RadioButton btn2 = new RadioButton("two player");
            
            RadioButton btn3 = new RadioButton("Zones");
            
            ToggleGroup radioGroup = new ToggleGroup();
            
            btn1.setToggleGroup(radioGroup);
            btn1.setSelected(true);
            btn2.setToggleGroup(radioGroup);
            btn3.setToggleGroup(radioGroup);
             
            vbox3.getChildren().addAll(lbName3,btn1,btn2,btn3);
            
            vbox3.setVisible(false);
            
            HBox.setHgrow(vbox3, Priority.ALWAYS);
            
            // the next two lines behave equally - try to comment the first line out and use the 2nd line
   
            hbox.setPadding(new Insets(20));
   
            hbox.getChildren().addAll(vbox, vbox2, vbox3);
            
            root.getChildren().add(hbox);
            
           
            Scene scene = new Scene(root, 800, 500); // the stack pane is the root node
            scene.getStylesheets().add("/select.css");
   
            primaryStage.setTitle("MotionComposer");
    
            primaryStage.setScene(scene);
   
            primaryStage.show();      
   
        }
    
    	public static void osccom() throws IOException 
    	{
    		//Object strReceiverName;
			OSCServer guiserver = OSCServer.newUsing(OSCServer.UDP, new InetSocketAddress( "127.0.0.1", 6562));
			guiserver.start();
			//guiclient = OSCClient.newUsing(OSCClient.UDP);
			guiserver.addOSCListener(new OSCListener() 
	    	 {
	    		 public void messageReceived( OSCMessage m, SocketAddress addr, long time )
		        	{
	    			
	     			 String receivedAddress = m.getName();
	  	        		//consoleMsg(receivedAddress);
	 	        		String strrAd= addr.toString();
	 	        		//consoleMsg(strrAd);
	 	        		
	 	        		try {
							msghandler(receivedAddress);
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
	     	 }
	    			 /*String receivedAddress = m.getName();
	 	        	//consoleMsg(receivedAddress);
		        		strrAd= addr.toString();
		        		System.out.println(receivedAddress);
		        		System.out.println(strrAd);
		        		messageArea.appendText(receivedAddress);*/

				private void msghandler(String address) throws IOException {
					// TODO Auto-generated method stub
					if(address.equals("/set/loaded"))	
						
					{
			        		messageArea.appendText(address);
			        		guiclient.setTarget(GUItoCM);
			        		guiclient.start();
			        		//guiclient.send(new OSCMessage( "/set/player/1/soundbank/1/list"));
					}
					else
					{
						messageArea.appendText(address);
					}
					
				}
		      
	    	 });
    	}
	
	public static void main(String args[]) throws IOException 
	{
		 osccom();
		 launch(args);
	}
	

}

