package edu.uob;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Paths;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

/** This class implements the DB server. */
public final class DBServer {

  private static final char END_OF_TRANSMISSION = 4;

  public static void main(String[] args) throws IOException {
    new DBServer(Paths.get(".").toAbsolutePath().toFile()).blockingListenOn(8888);
  }

  /**
   * KEEP this signature (i.e. {@code edu.uob.DBServer(File)}) otherwise we won't be able to mark
   * your submission correctly.
   *
   * <p>You MUST use the supplied {@code databaseDirectory} and only create/modify files in that
   * directory; it is an error to access files outside that directory.
   *
   * @param databaseDirectory The directory to use for storing any persistent database files such
   *     that starting a new instance of the server with the same directory will restore all
   *     databases. You may assume *exclusive* ownership of this directory for the lifetime of this
   *     server instance.
   */

  public DBServer(File databaseDirectory) {
    dbDirectory = databaseDirectory;
  }

  private final File dbDirectory;
  private File dbFile;
  private DBDatabase dataBase;
  private String messageToClient;

  /**
   * KEEP this signature (i.e. {@code edu.uob.DBServer.handleCommand(String)}) otherwise we won't be
   * able to mark your submission correctly.
   *
   * <p>This method handles all incoming DB commands and carry out the corresponding actions.
   */
  public String handleCommand(String command) {
    try {
      DBParser parser = new DBParser(command);
      new DBInterpreter(parser.getCommandToInterpret(), this);
    } catch (DBException e) { messageToClient = e.getMessage(); }
    return messageToClient;
  }

  // *** The methods below handle Read and Write functionality ***
  public DBTable readTableFromFile (File fileToRead) {
    DBTable tableToUse = null;
    try {
      FileReader reader = new FileReader(fileToRead);
      BufferedReader buffReader = new BufferedReader(reader);
      List<String> linesToRead = buffReader.lines().toList();
      tableToUse = new DBTable(fileToRead.getName().replace(".tab", ""));
      tableToUse.readLinesFromFile(linesToRead);
      buffReader.close();
    } catch (Exception e) { setMessageToClient("[ERROR] File read error"); }
    return tableToUse;
  }

  public void writeTableToFile(DBTable tableForWriting) {
    try {
      String fileName = tableForWriting.getTableName();
      fileName = dbFile.getPath().concat(File.separator+fileName+".tab");
      File fileToWrite = new File(fileName);
      if (fileToWrite.exists()) {
        FileWriter writer = new FileWriter(fileToWrite);
        writer.write(tableForWriting.tableToString());
        writer.flush();
        writer.close();
      }
    } catch (IOException IOException) {
      setMessageToClient("[ERROR] "+IOException.getMessage());
    }
  }

  /* *** General Server functionality *** */
  public void setMessageToClient (String message) { messageToClient = message; }

  public boolean isDatabaseNotInUse() { return dataBase == null; }

  public DBDatabase getDataBase() { return dataBase; }

  /* *** Server-Side Command implementations *** */
  public void useDB (String dbName) {
    try {
      dbFile = new File(dbDirectory.getPath() + File.separator + dbName);
      if (dbFile.isDirectory()) {
        dataBase = new DBDatabase(dbName);
        FileFilter filter = pathname -> pathname.getName().endsWith(".tab");
        File[] tableFileArray = dbFile.listFiles(filter);
        for (File tableFile : Objects.requireNonNull(tableFileArray)) {
          dataBase.addTableToArray(readTableFromFile(tableFile));
        }
        setMessageToClient("[OK]");
      }
      else { setMessageToClient(
                "[ERROR] Invalid Database name. Database does not exists in this directory");
      }
    } catch (Exception e) { setMessageToClient("[ERROR] File error"); }
  }

  public void createDB (String dbName) throws DBException {
    try {
      File newDB = new File(dbDirectory.getPath()+File.separator+dbName);
      if(!newDB.mkdir()) { throw new DBException.CreateFailed(dbName); }
      setMessageToClient("[OK]");
    }
    catch (SecurityException e) { setMessageToClient("[ERROR] Creation failed - permission denied"); }
  }

  public void createTable (String tableName, String line) throws DBException {
    File tabFile = new File(
            dbDirectory.getPath()+File.separator+
                     dataBase.getDbName()+File.separator+tableName+".tab");
    if (tabFile.exists()) { throw new DBException.AlreadyExists(tableName, false); }
    try {
      dataBase.createNewTable(tableName);
      if (!line.isEmpty()) {
        dataBase.getTable(tableName).initTokenizer();
        dataBase.getTable(tableName).readLinesFromFile(Collections.singletonList(line));
        dataBase.getTable(tableName).clearTokenizer();
      }
      writeTableToFile(dataBase.getTable(tableName));
      setMessageToClient("[OK]");
    }
    catch (SecurityException e) { setMessageToClient("[ERROR] Creation failed"); }
  }

  public void deleteTable (String tableName) throws DBException {
    dataBase.dropTable(tableName);
    try {
      File tabFile = new File(dbFile.getPath() + File.separator + tableName + ".tab");
      if (tabFile.exists() && tabFile.delete()) {
        setMessageToClient("[OK]"); }
      else { setMessageToClient("[ERROR] Drop was not successful. File does not exist."); }
    } catch (Exception e) { setMessageToClient("[ERROR] Unable to delete file - permission denied"); }
  }

  public void deleteDB (String dbName) throws DBException {
    if (Objects.requireNonNull(dataBase).getDbName().equalsIgnoreCase(dbName)) {
      dataBase = null; }
    setMessageToClient(deleteDBFile(dbName) ? "[OK]" : "[ERROR] Could not delete");
  }

  private boolean deleteDBFile (String dbName) {
    File[] list = dbDirectory.listFiles();
    for (File file : Objects.requireNonNull(list)) {
      if (file.getName().equalsIgnoreCase(dbName)) { return file.delete(); }
    }
    return false;
  }

  /* *** Testing specific methods *** */
  public File getDbDirectory() { return dbDirectory; }
  public String getMessageToClient() { return messageToClient; }

  //  === Methods below are there to facilitate server related operations. ===

  /**
   * Starts a *blocking* socket server listening for new connections. This method blocks until the
   * current thread is interrupted.
   *
   * <p>This method isn't used for marking. You shouldn't have to modify this method, but you can if
   * you want to.
   *
   * @param portNumber The port to listen on.
   * @throws IOException If any IO related operation fails.
   */
  public void blockingListenOn(int portNumber) throws IOException {
    try (ServerSocket s = new ServerSocket(portNumber)) {
      System.out.println("Server listening on port " + portNumber);
      while (!Thread.interrupted()) {
        try {
          blockingHandleConnection(s);
        } catch (IOException e) {
          System.err.println("Server encountered a non-fatal IO error:");
          e.printStackTrace();
          System.err.println("Continuing...");
        }
      }
    }
  }

  /**
   * Handles an incoming connection from the socket server.
   *
   * <p>This method isn't used for marking. You shouldn't have to modify this method, but you can if
   * * you want to.
   *
   * @param serverSocket The client socket to read/write from.
   * @throws IOException If any IO related operation fails.
   */
  private void blockingHandleConnection(ServerSocket serverSocket) throws IOException {
    try (Socket s = serverSocket.accept();
        BufferedReader reader = new BufferedReader(new InputStreamReader(s.getInputStream()));
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(s.getOutputStream()))) {

      System.out.println("Connection established: " + serverSocket.getInetAddress());
      while (!Thread.interrupted()) {
        String incomingCommand = reader.readLine();
        System.out.println("Received message: " + incomingCommand);
        String result = handleCommand(incomingCommand);
        writer.write(result);
        writer.write("\n" + END_OF_TRANSMISSION + "\n");
        writer.flush();
      }
    }
  }
}
