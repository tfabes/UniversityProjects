package edu.uob;

abstract class DBExpression {

    protected enum ETypes {
        BOOL,
        NULL,
        FLOAT,
        INT,
        ID,
        STRING
    }

    protected Boolean evaluation;
    public Boolean getEvaluation () throws DBException { return evaluation; }
    public void clearEvaluation() { evaluation = null; }
    public String getName() { return null; }
    abstract void accept(DBVisitor visitor) throws DBException;
    public void sendCondition (DBCondition condition) throws DBException {}

    static protected DBValue getValueAsType (DBToken token) throws DBException {
        String body = token.getTokenBody().toUpperCase();
        DBValue value;
        switch (token.getTokenType()) {
            case STRINGVALUE -> value = new DBValue.DBVString(token);
            case ALPHANUMERIC -> value = getAlphaNumValue(getAlphaNumType(body), token);
            default -> throw new DBException.InvalidValue(body);
        }
        return value;
    }

    static private ETypes getAlphaNumType (String body) {
        ETypes type;
        switch (body.toUpperCase()) {
            case "TRUE", "FALSE" -> type = ETypes.BOOL;
            case "NULL" -> type = ETypes.NULL;
            default -> {
                if (body.contains(".")) { type = ETypes.FLOAT; }
                else if (body.matches("^[0-9]+$")) { type = ETypes.INT; }
                else { type = ETypes.ID; }
            }
        }
        return type;
    }

    static private DBValue getAlphaNumValue (ETypes type, DBToken token) throws DBException {
        DBValue value;
        switch (type) {
            case BOOL -> value = new DBValue.DBVBool(token);
            case NULL -> value = new DBValue.DBVNull(token);
            case FLOAT -> value = new DBValue.DBVFloat(token);
            case INT -> value = new DBValue.DBVInteger(token);
            default -> value = new DBValue.DBVId(token);
        }
        return value;
    }


}
