package edu.uob;

import java.util.ArrayList;
import java.util.stream.IntStream;

public class DBInterpreter implements DBVisitor {

    DBServer server;
    ArrayList<DBValue> valueArr;
    ArrayList<DBList.DBNameValuePair> pairList;
    ArrayList<Integer> validRows;
    ArrayList<DBValue.DBVId> attributeList;
    int currentRow;
    String tableName;

    public DBInterpreter (DBCommand command, DBServer server) throws DBException {
        this.server = server;
        command.accept(this);
    }

    private void throwDBNotInUse() throws DBException.DBNotUsed { throw new DBException.DBNotUsed(); }

    private String attListToString (DBWildAttList.DBAttList list) {
        String asString = "";
        for (DBValue.DBVId att : list.attNames) { asString = asString.concat(att.entity +"\t"); }
        return asString;
    }

    @Override
    public void visitUseCMND(DBCommand.DBUse use) {
        server.useDB(use.nameID.entity);
    }

    @Override
    public void visitCreateCMND(DBCommand.DBCreate create) {
        try {
            String nameID = create.nameID.entity;
            switch (create.createType) {
                case "DATABASE" -> server.createDB(nameID);
                case "TABLE" -> {
                    if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
                    server.createTable(nameID, attListToString(create.attList));
                }
            }
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }

    @Override
    public void visitDropCMND(DBCommand.DBDrop drop) {
        try {
            switch (drop.dropType) {
                case "DATABASE" -> server.deleteDB(drop.nameID.entity);
                case "TABLE" -> {
                    if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
                    server.deleteTable(drop.nameID.entity);
                }
            }
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }

    @Override
    public void visitAlterCMND(DBCommand.DBAlter alter) {
        try {
            if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
            String nameID = alter.nameID.entity, attName = alter.attName.entity;
            if (attName.equalsIgnoreCase("id")) {
                throw new DBException("[ERROR] cannot alter id");
            }
            switch (alter.alterType) {
                case "ADD"  -> server.getDataBase().getTable(nameID).addAtt(attName);
                case "DROP" -> server.getDataBase().getTable(nameID).dropAtt(attName);
            }
            server.writeTableToFile(server.getDataBase().getTable(nameID));
            server.setMessageToClient("[OK]");
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }

    @Override
    public void visitInsertCMND(DBCommand.DBInsert insert) {
        try {
            if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
            insert.valueList.accept(this);
            DBTable table = server.getDataBase().getTable(insert.nameID.entity);
            table.initTokenizer();
            table.addValues(valueArr);
            table.clearTokenizer();
            server.writeTableToFile(table);
            server.setMessageToClient("[OK]");
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }

    @Override
    public void visitUpdateCMND(DBCommand.DBUpdate update) {
        try {
            if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
            tableName = update.nameID.entity;
            DBTable table = server.getDataBase().getTable(tableName);
            setValidRows(table, update.condition);
            update.nameValueList.accept(this);
            checkForIDUpdate();
            for (int row : validRows) {
                table.setValues(update.nameValueList.getNmVList(), row); }
            server.writeTableToFile(table);
            server.setMessageToClient("[OK]");
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }

    private void checkForIDUpdate() throws DBException {
        for (DBList.DBNameValuePair pair : pairList) {
            if (pair.getID().equalsIgnoreCase("id")) {
                throw new DBException("[ERROR] Cannot update id");
            }
        }
    }

    private void setValidRows(DBTable table, DBCondition condition) throws DBException {
        validRows = new ArrayList<>();
        int rows = table.getRows();
        for (currentRow = 1; currentRow < rows; currentRow++) {
            condition.accept(this);
            if (condition.getEvaluation()) { validRows.add(currentRow); }
            condition.clearEvaluation();
        }
    }

    private void reverseValidRows() {
        ArrayList<Integer> reversedRows = new ArrayList<>();
        for (int i = validRows.size() - 1; i >= 0; i--) {
            reversedRows.add(validRows.get(i));
        }
        validRows = reversedRows;
    }

    @Override
    public void visitDeleteCMND(DBCommand.DBDelete delete) {
        try {
            if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
            tableName = delete.nameID.entity;
            DBTable table = server.getDataBase().getTable(tableName);
            setValidRows(table, delete.condition);
            reverseValidRows();
            for (int row : validRows) { table.deleteRow(row); }
            server.writeTableToFile(table);
            server.setMessageToClient("[OK]");
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }


    @Override
    public void visitJoinCMND(DBCommand.DBJoin join) {
        try {
            if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
            DBTable firstTable = server.getDataBase().getTable(join.nameID.entity);
            DBTable secondTable = server.getDataBase().getTable(join.secondTableID.entity);
            String primaryKey = join.firstAttName.entity;
            String foreignKey = join.secondAttName.entity;
            DBTable joinTable = new DBTable("JoinTable");
            ArrayList<ArrayList<DBValue>> priSubTable = firstTable.getSubTable(primaryKey);
            ArrayList<ArrayList<DBValue>> secSubTable = secondTable.getSubTable(foreignKey);
            ArrayList<String> foreignEntities = secondTable.getColumnValues(foreignKey);
            ArrayList<Integer> joinRows = firstTable.getJoinRows(primaryKey, foreignEntities);
            priSubTable.get(0).addAll(secSubTable.get(0));
            for (int row : joinRows) {
                String entity = firstTable.getEntity(row, primaryKey).entity;
                int rowIndex = secondTable.getRowIndex(entity, foreignKey);
                priSubTable.get(row).addAll(secSubTable.get(rowIndex));
            }
            priSubTable.get(0).add(0, createID());
            joinTable.addRow(priSubTable.get(0));
            for (int j = 1; j < priSubTable.size(); j++) {
                priSubTable.get(j).add(0, createInt(Integer.toString(j)));
                joinTable.addRow(priSubTable.get(j));
            }
            server.setMessageToClient(mergeMessage(joinTable.tableToString()));
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }

    private String mergeMessage(String body) { return "[OK]\n"+body; }

    private DBValue.DBVId createID() {
        DBToken token = new DBToken(ETokenType.ALPHANUMERIC, "id");
        return new DBValue.DBVId(token);
    }

    private DBValue.DBVInteger createInt (String body) throws DBException {
        DBToken token = new DBToken(ETokenType.ALPHANUMERIC, body);
        return new DBValue.DBVInteger(token);
    }

    @Override
    public void visitSelectCMND(DBCommand.DBSelect select) {
        try {
            if (server.isDatabaseNotInUse()) { throwDBNotInUse(); }
            tableName = select.nameID.entity;
            DBTable table = server.getDataBase().getTable(tableName);
            if (!select.wildAttList.isWild()) { selectNotWild(select, table); }
            else {
                if (select.isConditionNull()) {
                    server.setMessageToClient("[OK]\n"+table.tableToString()); }
                else { selectWildCND(select, table); }
            }
        } catch (DBException e) { server.setMessageToClient(e.getMessage()); }
    }

    private void selectNotWild(DBCommand.DBSelect select, DBTable table) throws DBException {
        DBTable selectTable = new DBTable("selectTable");
        server.getDataBase().addTableToArray(selectTable);
        select.wildAttList.accept(this);
        if (select.isConditionNull()) { setAllRowsValid(table); }
        else { setValidRows(table, select.condition); }
        selectTable.addRow(table.getRow(0, attributeList));
        for (int row : validRows) { selectTable.addRow(table.getRow(row, attributeList)); }
        server.setMessageToClient("[OK]\n"+selectTable.tableToString());
    }

    private void setAllRowsValid (DBTable table) {
        validRows = new ArrayList<>();
        validRows.addAll(IntStream.rangeClosed(1, table.getRows() - 1).boxed().toList());
    }

    private void selectWildCND(DBCommand.DBSelect select, DBTable table) throws DBException {
        DBTable selectTable = new DBTable("selectTable");
        server.getDataBase().addTableToArray(selectTable);
        setValidRows(table, select.condition);
        selectTable.addRow(table.getFullRow(0));
        for (int row : validRows) { selectTable.addRow(table.getFullRow(row)); }
        server.setMessageToClient("[OK]\n"+selectTable.tableToString());
    }

    @Override
    public void visitCondition(DBCondition condition) throws DBException {
        if (condition.operator.equalsIgnoreCase("AND") ||
            condition.operator.equalsIgnoreCase("OR")) {
            condition.leftOperand.accept(this);
            condition.rightOperand.accept(this);
        }
        else {
            DBTable table = server.getDataBase().getTable(tableName);
            DBValue entity = table.getEntity(currentRow, condition.leftOperand.getName());
            condition.setOperand(entity);
        }
        condition.evaluateCondition();
    }

    @Override
    public void visitAttList(DBWildAttList.DBAttList attList) { attributeList = attList.attNames; }

    @Override
    public void visitValueList(DBList.DBValueList valueList) { valueArr = valueList.getValueList(); }

    @Override
    public void visitNameValuePair(DBList.DBNameValuePair nameValuePair) {
        if (!pairList.contains(nameValuePair)) { pairList.add(nameValuePair); } }

    @Override
    public void visitNameValueList(DBList.DBNameValueList nameValueList) { pairList = nameValueList.getNmVList(); }

}
