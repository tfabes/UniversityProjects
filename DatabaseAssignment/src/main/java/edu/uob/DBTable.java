package edu.uob;

import java.util.ArrayList;
import java.util.List;

public class DBTable {

    private String tableName;
    private final ArrayList<ArrayList<DBValue>> table;
    private int currentID;

    // Tokenizer for reading files and converting to DBValues
    // Init and clear called from Server
    private DBTokenizer tokenizer;

    public DBTable(String tableName) throws DBException {
        table = new ArrayList<>();
        setTableName(tableName);
        currentID = 1;
    }

    // Public helper methods
    public void setTableName (String tableName) throws DBException {
        checkValidName(tableName);
        this.tableName = tableName;
    }

    public String getTableName() { return tableName; }

    public void initTokenizer() { tokenizer = new DBTokenizer(); }

    public void clearTokenizer() { tokenizer = null; }

    public DBValue getEntity (int row, String attName) throws DBException {
        ArrayList<DBValue> record = table.get(row);
        int col = getAttributeColumn(attName);
        return record.get(col);
    }

    public int getRows() { return table.size(); }

    // Private helper methods
    private void checkValidName(String name) throws DBException {
        if (!name.matches("^[a-zA-Z0-9]*$")) { throw new DBException.InvalidName(tableName); }
    }

    private DBValue.DBVId stringToID (String att) throws DBException {
        checkValidName(att);
        return new DBValue.DBVId(new DBToken(ETokenType.ALPHANUMERIC, att));
    }

    private DBValue stringToValue (String value) throws DBException {
        DBToken token = new DBToken(tokenizer.assignTokenType(value), value);
        return DBValue.getValueAsType(token);
    }

    private int getAttributeColumn (String att) throws DBException {
        for (DBValue value : table.get(0)) {
            if (value.entity.equalsIgnoreCase(att)) { return table.get(0).indexOf(value); }
        }
        throw new DBException.AttNotFound(att);
    }

    /* *** Methods below implement in / out functionality for tables *** */
    public void readLinesFromFile (List<String> lines) throws DBException {
        initTokenizer();
        for (String line : lines) {
            if (!line.isBlank()) { readLineToTable(tabSplitLine(line)); }
        }
        clearTokenizer();
    }

    private String[] tabSplitLine (String line) { return line.split("\t"); }

    private void readLineToTable (String[] entities) throws DBException {
        table.add(new ArrayList<>());
        manageTableID(entities);
        for (String entity : entities) {
            if (!entity.isBlank()) {
                if (entity.matches("[^0-9'\"]*")) { entity = "'"+entity+"'"; }
                table.get(table.size()-1).add(stringToValue(entity));
            }
        }
    }

    private void manageTableID (String[] entities) throws DBException {
        if (table.size() == 1) {
            if (!entities[0].equalsIgnoreCase("id")) {
                table.get(0).add(stringToID("id"));
            }
        }
        else if (table.get(0).size() != entities.length) {
            table.get(table.size()-1).add(stringToValue(Integer.toString(currentID++)));
        }
        else {
            currentID = Integer.parseInt(entities[0]);
            currentID++;
        }
    }

    public String tableToString() {
        String tableAsString = "";
        for (ArrayList<DBValue> row : table) {
            tableAsString = tableAsString.concat(tabSeparateRecord(row)+"\n");
        }
        return tableAsString;
    }

    public String tabSeparateRecord(ArrayList<DBValue> rowIn) {
        String rowOut = "";
        for (DBValue value : rowIn) {
            rowOut = rowOut.concat(value.entity);
            if (rowIn.indexOf(value) < rowIn.size()-1) { rowOut = rowOut.concat("\t"); }
        }
        return rowOut;
    }

    /* Methods below implement command based functionality */
    public void addAtt (String attName) throws DBException {
        try { getAttributeColumn(attName); } catch (DBException.AttNotFound e) {
            for (ArrayList<DBValue> row : table) {
                if (table.indexOf(row) == 0) { row.add(stringToID(attName)); }
                else { row.add(stringToValue("")); }
            }
            return;
        }
        throw new DBException("[ERROR] Attribute already exists");
    }

    public void dropAtt (String attName) throws DBException {
        if (attName.equalsIgnoreCase("ID")) { throw new DBException("[ERROR] Cannot delete ID"); }
        int col = getAttributeColumn(attName);
        for (ArrayList<DBValue> row : table) { row.remove(col); }
    }

    public void addValues (ArrayList<DBValue> valArr) throws DBException {
        ArrayList<DBValue> idArr = new ArrayList<>();
        if (table.size() == 0 && !valArr.get(0).entity.equalsIgnoreCase("id")) {
            idArr.add(0, stringToValue("id"));
            idArr.addAll(valArr);
        }
        else {
            if (!valArr.get(0).entity.equals(Integer.toString(currentID))) {
                idArr.add(stringToValue(Integer.toString(currentID++)));
            } else { currentID++; }
            idArr.addAll(valArr);
            if (idArr.size() != table.get(0).size()) { throwValues(); }
        }
        table.add(idArr);
    }

    private void throwValues() throws DBException { throw new DBException("[ERROR] Incorrect number of values"); }

    public void setValues (ArrayList<DBList.DBNameValuePair> pairList, int row) throws DBException {
        for (DBList.DBNameValuePair pair : pairList) {
            table.get(row).set(getAttributeColumn(pair.getID()), pair.getValue());
        }
    }

    public void deleteRow (int row) { table.remove(row); }

    public ArrayList<DBValue> getRow (int row, ArrayList<DBValue.DBVId> names) throws DBException {
        ArrayList<DBValue> rowCopy = new ArrayList<>();
        ArrayList<Integer> cols = new ArrayList<>();
        for (DBValue.DBVId name : names) { cols.add(getAttributeColumn(name.entity)); }
        for (int i = 0; i < table.get(row).size(); i++) {
            if (cols.contains(i)) { rowCopy.add(table.get(row).get(i)); }
        }
        return rowCopy;
    }

    public ArrayList<DBValue> getFullRow (int row) { return table.get(row); }

    public void addRow (ArrayList<DBValue> rowToAdd) { table.add(rowToAdd); }

    public ArrayList<String> getColumnValues (String attName) throws DBException {
        int col = getAttributeColumn(attName);
        ArrayList<String> entityList = new ArrayList<>();
        for (int i = 1; i < table.size(); i++) { entityList.add(table.get(i).get(col).entity); }
        return entityList;
    }

    public ArrayList<Integer> getJoinRows (String attName, ArrayList<String> entityList) throws DBException {
        int col = getAttributeColumn(attName);
        ArrayList<Integer> joinRows = new ArrayList<>();
        for (int i = 1; i < table.size(); i++) {
            if (entityList.contains(table.get(i).get(col).entity)) { joinRows.add(i); }
        }
        return joinRows;
    }

    public ArrayList<ArrayList<DBValue>> getSubTable (String key) throws DBException {
        int col = getAttributeColumn(key);
        ArrayList<ArrayList<DBValue>> subTable = new ArrayList<>();
        for (ArrayList<DBValue> row : table) {
            subTable.add(new ArrayList<>());
            for (DBValue value : row) {
                if (row.indexOf(value) != 0 && row.indexOf(value) != col) {
                    subTable.get(table.indexOf(row)).add(value);
                }
            }
        }
        return subTable;
    }

    public int getRowIndex (String entity, String attName) throws DBException {
        int col = getAttributeColumn(attName);
        for (int i = 0; i < table.size(); i++) {
            if (table.get(i).get(col).entity.equals(entity)) { return i; }
        }
        throw new DBException("[ERROR] Could not find matching entity");
    }

}
