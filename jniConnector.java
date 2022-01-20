
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;

class  eventRecord {
    public String id;  
    public String logSource;  
    public String eventType; 
    public String eventTime; 
    public String eventSource; 
    public String eventData;

    public  eventRecord(){
        id           = new String("");
        logSource     = new String("");
        eventType    = new String("");
        eventTime      = new String("");
        eventSource = new String("");
        eventData = new String("");
    }
    public String toString() {   
        return "[" + id+ ","+ logSource + "," + eventTime +"]";   
    }
}
public class jniConnector {
	static String url = "jdbc:mysql://localhost:3306/dbWindowsLogs";
    static String user = "root";
    static String password = "gunal@333";
    static Connection con = makeConnection();
public static ArrayList<Object> prevMaxTime() throws SQLException
{
    /*Select recordNumber,max(eventTime) from dbwindowslogs.eventlogs where eventTime = (select max(eventTime) from dbwindowslogs.eventlogs)*/
    String sql ="Select recordNumber,max(eventTime) as maxTime from dbwindowslogs.eventlogs where eventTime = (select max(eventTime) from dbwindowslogs.eventlogs)";
    PreparedStatement stmt = con.prepareStatement(sql);
    ResultSet rs = stmt.executeQuery(sql);
    java.sql.Timestamp time = null;
    int recordNumber = 0;
    if(rs.next())
    {
        time = rs.getTimestamp("maxTime");
        recordNumber =rs.getInt("recordNumber");
    }
    ArrayList<Object> out = new ArrayList<>();
    out.add(time);
    out.add(recordNumber);
    return out;
}
public static Connection makeConnection()
{
    Connection con = null;
    try {
		Class.forName("com.mysql.cj.jdbc.Driver");
	} catch (ClassNotFoundException e1) {
		// TODO Auto-generated catch block
		e1.printStackTrace();
	}
    try {
        con = DriverManager.getConnection(url, user, password);
    } catch (SQLException e) {
        e.printStackTrace();
    }
    return con;
}
    static {
        System.loadLibrary("native");
    }
    
    private native static eventRecord[] getEventRecords(String sourceName);
    public static void main(String[] args) throws SQLException {
        String[] sources = {"Application","System"};
        int i=0,newRecords=0;
        String sqlState = "INSERT INTO eventlogs(sourceName,recordNumber,eventID,eventTime,eventType,eventData,eventSource) VALUES (?,?,?,?,?,?,?)";
        PreparedStatement statement = con.prepareStatement(sqlState);
        ArrayList<Object> res =prevMaxTime();
        java.sql.Timestamp preMaxTime = (java.sql.Timestamp)res.get(0);
        int recordNumber = (int)res.get(1);
        System.out.println("Last record identifiers: "+recordNumber+" : "+preMaxTime);
        for (String string : sources) {
        try{
        eventRecord[] records =  getEventRecords(string);
        for(eventRecord record:records){
            i++;
            java.sql.Timestamp rTime =  new java.sql.Timestamp(new Date(Long.valueOf(record.eventTime).longValue()*1000L).getTime());
            if(rTime.before(preMaxTime))
            {
                continue;
            }
            if((!rTime.before(preMaxTime)&&!rTime.after(preMaxTime))&&i<=recordNumber)
            {
                continue;
            }
            newRecords++;
            System.out.println(i+". ID :"+record.id);
            System.out.println("Log source :"+record.logSource);
            System.out.println("Event type:"+record.eventType);
            System.out.println("Event time:"+rTime);
            System.out.println("Event source:"+record.eventSource);
            System.out.println("Event Data: "+record.eventData);
            statement.setString(1,record.logSource);
            statement.setInt(2,i);
            statement.setInt(3,Integer.parseInt(record.id));
            statement.setTimestamp(4,rTime);
            statement.setString(5,record.eventType);
            statement.setString(6,record.eventData);
            statement.setString(7, record.eventSource);
            statement.addBatch();
        }
        }
        catch(UnsatisfiedLinkError e) {
            System.out.println("Couldn't call native function.\n" + e);    
        }
    }
    statement.executeBatch();
    System.out.println("New Records added : "+newRecords);
    }

    // Declare a native method sayHello() that receives no arguments and returns void
    
}