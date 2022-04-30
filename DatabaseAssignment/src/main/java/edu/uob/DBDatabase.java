package edu.uob;

import java.util.ArrayList;

public class DBDatabase {

    private final ArrayList<DBTable> tableArray;
    private final String dbName;

    public DBDatabase (String dbName) {
        this.dbName = dbName;
        tableArray = new ArrayList<>();
    }

    public String getDbName() { return dbName; }

    public void addTableToArray (DBTable tableToAdd) { tableArray.add(tableToAdd); }

    public void createNewTable (String tableName) throws DBException {
        tableArray.add(new DBTable(tableName));
    }

    public void dropTable (String tableName) throws DBException { tableArray.remove(getTable(tableName)); }

    public DBTable getTable (String tableName) throws DBException {
        for (DBTable table : tableArray) {
            if (table.getTableName().equals(tableName)) { return table; }
        }
        throw new DBException("[ERROR] Table not found");
    }
}
