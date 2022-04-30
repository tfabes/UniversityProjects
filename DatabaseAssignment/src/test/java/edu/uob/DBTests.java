package edu.uob;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import java.io.File;
import java.io.FileWriter;

import static org.junit.jupiter.api.Assertions.*;

// PLEASE READ:
// The tests in this file will fail by default for a template skeleton, your job is to pass them
// and maybe write some more, read up on how to write tests at
// https://junit.org/junit5/docs/current/user-guide/#writing-tests
final class DBTests {

  private DBServer server;

  // we make a new server for every @Test (i.e. this method runs before every @Test test case)
  @BeforeEach
  void setup(@TempDir File dbDir) {
    // Notice the @TempDir annotation, this instructs JUnit to create a new temp directory somewhere
    // and proceeds to *delete* that directory when the test finishes.
    // You can read the specifics of this at
    // https://junit.org/junit5/docs/5.4.2/api/org/junit/jupiter/api/io/TempDir.html

    // If you want to inspect the content of the directory during/after a test run for debugging,
    // simply replace `dbDir` here with your own File instance that points to somewhere you know.
    // IMPORTANT: If you do this, make sure you rerun the tests using `dbDir` again to make sure it
    // still works and keep it that way for the submission.

    server = new DBServer(dbDir);
  }

  String courses = """
          id	course	students	lead
          1	CompSci	25	teacherA
          2	French	20	teacherB
          3	English	30	teacherC
          """;

  String schools = """
          id	name	intake	address	distance
          1	schoolA	5000	upper street	250
          2	schoolB	250	lower street	500
          3	schoolC	1000	middle street	750
          4	schoolD	2000	down	1000
          5	schoolE	3000	up street	300
          """;

  String students = """
          id	firstname	surname	course	fees	school
          1	bob	bobson	CompSci	500	schoolA
          2	tom	tomson	CompSci	500	schoolB
          3	cliff	cliffson	French	250	schoolA
          4	george	georgeson	English	650	schoolA
          5	leo	lesson	English	550	schoolC
          """;

  private void setUpFullDB() {
    File db = setUpTestDBDir();
    writeTable(db, courses, "courses");
    writeTable(db, schools, "schools");
    writeTable(db, students, "students");
  }

  private File setUpTestDBDir() {
    try {
      File dbDirectory = server.getDbDirectory();
      File testDB = new File(dbDirectory.getPath() + File.separator + "testDB");
      assertTrue(testDB.mkdir());
      return testDB;
    } catch (Exception e) { System.out.println(e.getMessage()); }
    return null;
  }

  private void writeTable(File testDB, String strLit, String name) {
    try {
      File testTable = new File(testDB.getPath()+File.separator+name+".tab");
      assertTrue(testTable.createNewFile());
      FileWriter writer = new FileWriter(testTable);
      writer.write(strLit);
      writer.flush();
      writer.close();
    } catch (Exception e) { System.out.println(e.getMessage()); }
  }

  private String runQuery(String message) throws DBException {
    DBParser parser = new DBParser(message);
    new DBInterpreter(parser.getCommandToInterpret(), server);
    return server.getMessageToClient();
  }


  // Here's a basic test for spawning a new server and sending an invalid command,
  // the spec dictates that the server respond with something that starts with `[ERROR]`
  @Test
  void testInvalidCommandIsAnError() {
    assertTrue(server.handleCommand("foo").startsWith("[ERROR]"));
  }

  // Add more unit tests or integration tests here.
  // Unit tests would test individual methods or classes whereas integration tests are geared
  // towards a specific usecase (i.e. creating a table and inserting rows and asserting whether the
  // rows are actually inserted)


  /* *** My Integration Testing *** */

  @Test
  void testUseDB() throws DBException {
    setUpFullDB();
    assertEquals(runQuery("USE testDB ;"), "[OK]");
  }

  @Test
  void testSelect() throws DBException {
    setUpFullDB();
    assertEquals(runQuery("USE testDB ;"), "[OK]");
    assertEquals(runQuery("select * from schools ;"), "[OK]\n"+schools);
    assertEquals(runQuery("select * from students ;"), "[OK]\n"+students);
    assertEquals(runQuery("select * from courses ;"), "[OK]\n"+courses);
  }

  @Test
  void testErrorSelect() {
    setUpFullDB();
    try {
      assertTrue(runQuery("USE unknown ;").startsWith("[ERROR]"));
      assertTrue(runQuery("USE ;").startsWith("[ERROR]"));
      assertTrue(runQuery("USE testDB").startsWith("[ERROR]"));
      assertTrue(runQuery("testDB ;").startsWith("[ERROR]"));
      assertTrue(runQuery("USEtestDB;").startsWith("[ERROR]"));
      assertTrue(runQuery("create testDB ;").startsWith("[ERROR]"));
    } catch (DBException e) { assertInstanceOf(DBException.CommandException.class, e); }
  }

  @Test
  void testUpdate() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertEquals(runQuery("UPDATE schools SET name = 'new school' WHERE id == 1 ;"), "[OK]");
      assertEquals(runQuery("SELECT name from schools where id == 1 ;"), "[OK]\nname\nnew school\n");
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  @Test
  void testDoubleConditionUpdate() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertEquals(runQuery("UPDATE schools SET name = 'new school' WHERE (id==1)and(name like 'school') ;"), "[OK]");
      assertEquals(runQuery("SELECT name from schools where id == 1 ;"), "[OK]\nname\nnew school\n");
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  @Test
  void testSelectFourConditions() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertEquals(runQuery(
              "SELECT name from schools where ((id==1)and(name=='schoolA'))and((intake>50)and(distance<2000));"),
              "[OK]\nname\nschoolA\n");
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  @Test
  void testJoinTable() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertEquals(runQuery("alter table students drop firstname;"), "[OK]");
      assertEquals(runQuery("alter table students drop surname;"), "[OK]");
      assertEquals(runQuery("alter table students drop course;"), "[OK]");
      assertEquals(runQuery("alter table students drop fees;"), "[OK]");
      assertEquals(runQuery("delete from students where id > 1;"), "[OK]");
      assertEquals(runQuery("alter table schools drop intake;"), "[OK]");
      assertEquals(runQuery("alter table schools drop address;"), "[OK]");
      assertEquals(runQuery("alter table schools drop distance;"), "[OK]");
      assertEquals(runQuery("delete from schools where id > 1;"), "[OK]");
      assertEquals(runQuery("join students and schools on school and name;"),
              "[OK]\nid\n1\n");
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  @Test
  void testDropTable() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertEquals(runQuery("DROP table schools; "), "[OK]");
      assertTrue(runQuery("select * from schools ;").startsWith("[ERROR]"));
    } catch (DBException e) { assertInstanceOf(DBException.InvalidName.class, e); }
  }

  @Test
  void testCreateTable() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertEquals(runQuery("DROP table schools; "), "[OK]");
      assertEquals(runQuery("create table schools;"), "[OK]");
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  @Test
  void testFailCreateTable() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertTrue(runQuery("create table schools;").startsWith("[ERROR]"));
    } catch (DBException e) { assertInstanceOf(DBException.AlreadyExists.class, e); }
  }

  @Test
  void testCreateDB() {
    try {
      assertEquals(runQuery("CREATE DATABASE newDB ;"), "[OK]");
      assertEquals(runQuery("USE newDB ;"), "[OK]");
      assertEquals(runQuery("CREATE TABLE newTable ;"), "[OK]");
      assertEquals(runQuery("INSERT INTO newTable VALUES('name');"), "[OK]");
      assertEquals(runQuery("INSERT INTO newTable VALUES('bob');"), "[OK]");
      assertEquals(runQuery("select * from newTable where id == 1;"),
              "[OK]\nid\tname\n1\tbob\n");
      assertEquals(runQuery("select * from newTable where name > 'cob';"),
              "[OK]\nid\tname\n");
      assertEquals(runQuery("delete from newTable where (id==1)and(name>'aob');"), "[OK]");
      assertEquals(runQuery("select * from newTable;"),
              "[OK]\nid\tname\n");
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  @Test
  void testDeleteAllRowsAtt() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertEquals(runQuery("delete from schools where id > 0 ;"), "[OK]");
      assertEquals(runQuery("alter table schools drop name ;"), "[OK]");
      assertEquals(runQuery("alter table schools drop intake ;"), "[OK]");
      assertEquals(runQuery("alter table schools drop address ;"), "[OK]");
      assertEquals(runQuery("alter table schools drop distance ;"), "[OK]");
      assertEquals(runQuery("select * from schools ;"), "[OK]\nid\n");
      assertEquals(runQuery("drop table schools ;"), "[OK]");
      assertTrue(runQuery("select * from schools ;").startsWith("[ERROR]"));
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  @Test
  void testSelectUnknownTable() {
    setUpFullDB();
    try {
      assertEquals(runQuery("USE testDB ;"), "[OK]");
      assertTrue(runQuery("select * from abc ;").startsWith("[ERROR]"));
    } catch (DBException e) { System.out.println(e.getMessage()); }
  }

  // *** Tokenizer Unit Testing *** //

  @Test
  void testTokenizeCommands() {
    DBTokenizer tokenizer = new DBTokenizer();
    try {
      assertEquals(tokenizer.assignTokenType("USE"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("create"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("DROP"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("AltER"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("Insert"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("update"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("delETE"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("jOIn"), ETokenType.COMMAND);
      assertEquals(tokenizer.assignTokenType("seLecT"), ETokenType.COMMAND);
    } catch (DBException e) { System.out.println("Exception incorrectly thrown in test"); }
  }

  @Test
  void testTokenizeSyntax() {
    DBTokenizer tokenizer = new DBTokenizer();
    try {
      assertEquals(tokenizer.assignTokenType("WHERE"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType("FROM"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType("AND"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType("SET"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType("ON"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType("ADD"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType("("), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType(")"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType(","), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType(";"), ETokenType.SYNTAX);
      assertEquals(tokenizer.assignTokenType("*"), ETokenType.SYNTAX);
    } catch (DBException e) { System.out.println("Exception incorrectly thrown in test"); }
  }

  @Test
  void testStructAlterTokens() {
    DBTokenizer tokenizer = new DBTokenizer();
    try {
      assertEquals(tokenizer.assignTokenType("DATABASE"), ETokenType.STRUCTURE);
      assertEquals(tokenizer.assignTokenType("database"), ETokenType.STRUCTURE);
      assertEquals(tokenizer.assignTokenType("daTAbaSE"), ETokenType.STRUCTURE);
      assertEquals(tokenizer.assignTokenType("table"), ETokenType.STRUCTURE);
      assertEquals(tokenizer.assignTokenType("TABLE"), ETokenType.STRUCTURE);
    } catch (DBException e) { System.out.println("Exception incorrectly thrown in test"); }
  }

  @Test
  void testStringTokens() {
    DBTokenizer tokenizer = new DBTokenizer();
    try {
      assertEquals(tokenizer.assignTokenType("' a '"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("'a b 1 2 3   '"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("'$@&'"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("'name'"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("'name1234'"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("'1234 name '"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("''"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("' '"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("'   name    '"), ETokenType.STRINGVALUE);
      assertEquals(tokenizer.assignTokenType("'{}{}{}{}'"), ETokenType.STRINGVALUE);
    } catch (DBException e) { System.out.println("Exception incorrectly thrown in test"); }
  }

  @Test
  void testSplitOutSingleQuotes() {
    DBTokenizer tokenizer = new DBTokenizer();
    String testString = "DELETE FROM name WHERE id=='bob surname';";
    assertEquals("'bob surname'", tokenizer.splitOnLiteralString(testString)[1]);
    testString = "DELETE FROM name WHERE id == '1234 abc56' ;";
    assertEquals("'1234 abc56'", tokenizer.splitOnLiteralString(testString)[1]);
    testString = "DELETE FROM name WHERE id=='   ';";
    assertEquals("'   '", tokenizer.splitOnLiteralString(testString)[1]);
    testString = "DELETE FROM name WHERE id    ==    ' ab'   ;    ";
    assertEquals("' ab'", tokenizer.splitOnLiteralString(testString)[1]);
    testString = "DELETE FROM name WHERE id    ==    ' ab cd ef '   ;    ";
    assertEquals("' ab cd ef '", tokenizer.splitOnLiteralString(testString)[1]);
    testString = "DELETE FROM name WHERE id==' ab cd ef. 28 ;528 @sje ';";
    assertEquals("' ab cd ef. 28 ;528 @sje '", tokenizer.splitOnLiteralString(testString)[1]);
    testString = "This='white space',that='hello' WHERE (id='5')AND";
    assertEquals("'white space'", tokenizer.splitOnLiteralString(testString)[1]);
    assertEquals("'hello'", tokenizer.splitOnLiteralString(testString)[3]);
    assertEquals("'5'", tokenizer.splitOnLiteralString(testString)[5]);
    testString = "DELETE '' REGEX";
    assertEquals("''", tokenizer.splitOnLiteralString(testString)[1]);
  }

  @Test
  void testSplitOutOperators() {
    DBTokenizer tokenizer = new DBTokenizer();
    String testString = "DELETE FROM name WHERE name  ==  'Bob Peterson'";
    assertEquals("==", tokenizer.splitOnOperators(testString)[1]);
    testString = "DELETE FROM name WHERE name!='Bob Peterson'";
    assertEquals("!=", tokenizer.splitOnOperators(testString)[1]);
    testString = "DELETE FROM name WHERE name >=    'Bob Peterson'";
    assertEquals(">=", tokenizer.splitOnOperators(testString)[1]);
    testString = "DELETE FROM name WHERE name   >'  Bob Peterson'";
    assertEquals(">", tokenizer.splitOnOperators(testString)[1]);
    testString = "DELETE FROM name WHERE name<  'Bob Peterson'";
    assertEquals("<", tokenizer.splitOnOperators(testString)[1]);
    testString = "DELETE FROM name WHERE name <= 'Bob Peterson'";
    assertEquals("<=", tokenizer.splitOnOperators(testString)[1]);
    testString = "UPDATE table SET id=5 WHERE name=='Bob'";
    assertEquals("=", tokenizer.splitOnOperators(testString)[1]);
    assertEquals("==", tokenizer.splitOnOperators(testString)[3]);
    testString = "DELETE FROM name WHERE (name<='Bob Peterson')AND(name=='Bob'";
    assertEquals("<=", tokenizer.splitOnOperators(testString)[1]);
    assertEquals("==", tokenizer.splitOnOperators(testString)[3]);
  }

  @Test
  void testSplitOutKeyWords() {
    DBTokenizer tokenizer = new DBTokenizer();
    String testString = "USE testDB ;";
    assertEquals(";", tokenizer.splitOnKeyWords(testString)[1]);
    testString = "USE testDB;";
    assertEquals(";", tokenizer.splitOnKeyWords(testString)[1]);
  }

  @Test
  void testStringLiterals() {
    assertThrows(DBException.InvalidString.class, () ->
            new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "1234")));
    assertThrows(DBException.InvalidString.class, () ->
            new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "'1234'")));
    assertThrows(DBException.InvalidString.class, () ->
            new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "'a1'")));
    assertThrows(DBException.InvalidString.class, () ->
            new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "'\t'")));
    assertThrows(DBException.InvalidString.class, () ->
            new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "\"\t'")));
    assertThrows(DBException.InvalidString.class, () ->
            new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "£")));
    assertThrows(DBException.InvalidString.class, () ->
            new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "|")));
    assertDoesNotThrow(() -> {
      new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "string"));});
    assertDoesNotThrow(() -> {
      new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "this is a string"));});
    assertDoesNotThrow(() -> {
      new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "'@()@()*$@_(*!&*$($'"));});
    assertDoesNotThrow(() -> {
      new DBValue.DBVString(new DBToken(ETokenType.STRINGVALUE, "'a B C .... ; def '"));});
  }


}
