package edu.uob;

import java.util.ArrayList;

public class DBParser {

    private final DBTokenizer tokenizer;
    private ECommand commandType;
    private final DBCommand commandToInterpret;

    public DBParser(String command) throws DBException {
        tokenizer = new DBTokenizer(command);
        setCommandType(getNextToken().getTokenBody());
        commandToInterpret = parseCommand();
        if (isNotEnd()) { throw new DBException("[ERROR] Invalid statement"); }
    }

    /* *** General Parser functionality *** */
    public DBCommand getCommandToInterpret() { return commandToInterpret; }

    private DBToken getNextToken() { return tokenizer.getTokenThenIncrement(); }

    private void ignoreToken() { tokenizer.ignoreToken(); }

    private boolean isNotEnd() { return !tokenizer.isLastToken(); }

    private boolean matchNextTokenBody (String body, boolean consume) {
        if (consume) { return getNextToken().getTokenBody().toUpperCase().equals(body); }
        return tokenizer.checkNextToken().getTokenBody().toUpperCase().equals(body);
    }

    private boolean isInvalidID() {
        return !(tokenizer.checkNextToken().getTokenType() == ETokenType.ALPHANUMERIC &&
                 tokenizer.checkNextToken().getTokenBody().matches("^[a-zA-Z0-9]*$")); }

    // Command identification and parser calls
    private void setCommandType (String body) throws DBException {
        try { commandType = ECommand.valueOf(body.toUpperCase()); }
        catch (IllegalArgumentException exception) {
            throw new DBException("[ERROR] Invalid Command: "+body); }
    }

    private DBCommand parseCommand() throws DBException {
        DBCommand command;
        switch (commandType) {
            case USE -> command = parseUse();
            case CREATE -> command = parseCreate();
            case DROP -> command = parseDrop();
            case ALTER -> command = parseAlter();
            case INSERT -> command = parseInsert();
            case UPDATE -> command = parseUpdate();
            case DELETE -> command = parseDelete();
            case JOIN -> command = parseJoin();
            case SELECT -> command = parseSelect();
            default -> throw new DBException("[ERROR] Unknown command");
        }
        return command;
    }

    // Command Parsing
    private void throwCMND(String type) throws DBException { throw new DBException.CommandException(type); }

    private DBCommand.DBUse parseUse() throws DBException {
        if (tokenizer.getNumberOfTokens() != 3) { throwCMND("USE"); }
        return new DBCommand.DBUse(parseID());
    }

    private DBCommand.DBCreate parseCreate() throws DBException {
        String type = parseStruct("CREATE");
        DBValue.DBVId createID = parseID();
        DBWildAttList.DBAttList attList = new DBWildAttList.DBAttList(new ArrayList<>());
        if (isNotEnd()) {
            if (!matchNextTokenBody("(", true)) { throwCMND("CREATE"); }
            attList = parseAttList();
            if (!matchNextTokenBody(")", true)) { throwCMND("CREATE"); }
        }
        return new DBCommand.DBCreate(createID, type, attList);
    }

    private DBCommand.DBDrop parseDrop() throws DBException {
        String type = parseStruct("DROP");
        DBValue.DBVId createID = parseID();
        return new DBCommand.DBDrop(createID, type);
    }

    private DBCommand.DBAlter parseAlter() throws DBException {
        if (!matchNextTokenBody("TABLE", true)) { throwCMND("ALTER"); }
        DBValue.DBVId alterID = parseID();
        String type = parseAlterType();
        DBValue.DBVId attName = parseID();
        return new DBCommand.DBAlter(alterID, type, attName);
    }

    private DBCommand.DBInsert parseInsert() throws DBException {
        if (!matchNextTokenBody("INTO", true)) { throwCMND("INSERT"); }
        DBValue.DBVId insertID = parseID();
        if (!matchNextTokenBody("VALUES", true)) { throwCMND("INSERT"); }
        if (!matchNextTokenBody("(", true)) { throwCMND("INSERT"); }
        DBList.DBValueList valueList = parseValueList();
        if (!matchNextTokenBody(")", true)) { throwCMND("INSERT"); }
        return new DBCommand.DBInsert(insertID, valueList);
    }

    private DBCommand.DBUpdate parseUpdate() throws DBException {
        DBValue.DBVId updateID = parseID();
        if (!matchNextTokenBody("SET", true)) { throwCMND("UPDATE"); }
        DBList.DBNameValueList nmvList = parseNameValueList();
        if (!matchNextTokenBody("WHERE", true)) { throwCMND("UPDATE"); }
        DBCondition cnd = parseCondition();
        return new DBCommand.DBUpdate(updateID, nmvList, cnd);
    }

    private DBCommand.DBDelete parseDelete() throws DBException {
        if (!matchNextTokenBody("FROM", true)) { throwCMND("DELETE"); }
        DBValue.DBVId deleteID = parseID();
        if (!matchNextTokenBody("WHERE", true)) { throwCMND("DELETE"); }
        DBCondition condition = parseCondition();
        return new DBCommand.DBDelete(deleteID, condition);
    }

    private DBCommand.DBJoin parseJoin() throws DBException {
        DBValue.DBVId[] idArr = new DBValue.DBVId[4];
        idArr[0] = new DBValue.DBVId(getNextToken());
        if (!matchNextTokenBody("AND", true)) { throwCMND("JOIN"); }
        idArr[1] = new DBValue.DBVId(getNextToken());
        if (!matchNextTokenBody("ON", true)) { throwCMND("JOIN"); }
        idArr[2] = new DBValue.DBVId(getNextToken());
        if (!matchNextTokenBody("AND", true)) { throwCMND("JOIN"); }
        idArr[3] = new DBValue.DBVId(getNextToken());
        return new DBCommand.DBJoin(idArr);
    }

    private DBCommand.DBSelect parseSelect() throws DBException {
        DBWildAttList wildAttList = parseWildAttList();
        if (!matchNextTokenBody("FROM", true)) { throwCMND("SELECT"); }
        DBValue.DBVId selectID = new DBValue.DBVId(getNextToken());
        DBCondition condition = null;
        if (isNotEnd()) {
            if (!matchNextTokenBody("WHERE", true)) { throwCMND("SELECT"); }
            condition = parseCondition(); }
        return new DBCommand.DBSelect(selectID, wildAttList, condition);
    }

    // Keyword parsing
    private String parseStruct(String cmnd) throws DBException {
        String type = getNextToken().getTokenBody();
        if (type.equals("TABLE") || type.equals("DATABASE")) { return type; }
        throw new DBException.StructException(cmnd);
    }

    private String parseAlterType() throws DBException {
        String type = getNextToken().getTokenBody();
        if (type.equals("ADD") || type.equals("DROP")) { return type; }
        throw new DBException.AlterException();
    }

    // Expression parsing
    private DBCondition parseCondition() throws DBException {
        DBExpression leftOperand, rightOperand;
        DBToken operator;
        if (matchNextTokenBody("(", false)) {
            ignoreToken();
            leftOperand = parseCondition();
            if (!matchNextTokenBody(")", true)) { throwCondition(); }
            operator = getNextToken();
            if (!matchNextTokenBody("(", true)) { throwCondition(); }
            rightOperand = parseCondition();
            if (!matchNextTokenBody(")", true)) { throwCondition(); } }
        else {
            leftOperand = parseID();
            operator = getNextToken();
            rightOperand = DBValue.getValueAsType(getNextToken()); }
        return new DBCondition(operator, leftOperand, rightOperand);
    }

    private void throwCondition() throws DBException { throw new DBException.InvalidCondition(); }

    private DBValue.DBVId parseID() throws DBException {
        if (isInvalidID()) { throw new DBException.IDException(); }
        DBToken token = getNextToken();
        return new DBValue.DBVId(token);
    }

    private DBWildAttList.DBAttList parseAttList() throws DBException {
        ArrayList<DBToken> tokenList = new ArrayList<>();
        checkThrowID();
        tokenList.add(getNextToken());
        while (matchNextTokenBody(",", false)) {
            ignoreToken();
            checkThrowID();
            tokenList.add(getNextToken());
        }
        return new DBWildAttList.DBAttList(tokenList);
    }

    private void checkThrowID() throws DBException {
        if(isInvalidID()) { throw new DBException.IDException(); } }

    private DBWildAttList parseWildAttList() throws DBException {
        if (matchNextTokenBody("*", false)) {
            return new DBWildAttList.DBWild(getNextToken()); }
        return parseAttList();
    }

    private DBList.DBValueList parseValueList() throws DBException {
        ArrayList<DBToken> tokenList = new ArrayList<>();
        tokenList.add(getNextToken());
        while (matchNextTokenBody(",", false)) {
            ignoreToken();
            tokenList.add(getNextToken());
        }
        return new DBList.DBValueList(tokenList);
    }

    private DBList.DBNameValuePair parseNameValuePair() throws DBException {
        DBToken attToken = getNextToken();
        if (!matchNextTokenBody("=", true)) { throwCMND("UPDATE"); }
        return new DBList.DBNameValuePair(attToken, getNextToken());
    }

    private DBList.DBNameValueList parseNameValueList() throws DBException {
        ArrayList<DBList.DBNameValuePair> pairList = new ArrayList<>();
        pairList.add(parseNameValuePair());
        while (matchNextTokenBody(",", false)) {
            ignoreToken();
            pairList.add(parseNameValuePair());
        }
        return new DBList.DBNameValueList(pairList);
    }

}
