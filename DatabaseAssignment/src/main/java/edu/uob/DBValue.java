package edu.uob;

abstract class DBValue extends DBExpression {

    public DBValue (DBToken token) { entity = token.getTokenBody(); }
    void accept(DBVisitor visitor) {}
    protected final String entity;

    static class DBVId extends DBValue {
        public DBVId (DBToken token) { super(token); }
        @Override public String getName() { return entity; }
    }

    static class DBVNull extends DBValue {
        public DBVNull (DBToken token) { super(token); }
        @Override public void sendCondition (DBCondition condition) { condition.addString(entity); }
    }

    static class DBVBool extends DBValue {
        public DBVBool (DBToken token) {
            super(token);
            bool = Boolean.valueOf(entity.toLowerCase());
        }
        private final Boolean bool;
        @Override public void sendCondition (DBCondition condition) { condition.addBool(bool); }
    }

    static class DBVFloat extends DBValue {
        public DBVFloat (DBToken token) throws DBException {
            super(token);
            try {
                checkFloat();
                floatLiteral = Float.parseFloat(entity);
            }
            catch (Exception e) { throw new DBException.InvalidFloat(entity); }
        }
        private void checkFloat() throws DBException.InvalidFloat {
            double floatCheck = Double.parseDouble(entity);
            if (floatCheck > Float.MAX_VALUE || floatCheck < Float.MIN_VALUE ||
                (Math.abs(floatCheck) < Float.MIN_NORMAL)) {
                throw new DBException.InvalidFloat(entity); }
        }
        private final Float floatLiteral;
        @Override public void sendCondition (DBCondition condition) { condition.addFloat(floatLiteral); }
    }

    static class DBVInteger extends DBValue {
        public DBVInteger (DBToken token) throws DBException {
            super(token);
            try {
                checkInteger();
                intLiteral = Integer.parseInt(entity);
            }
            catch (Exception e) { throw new DBException.InvalidInteger(entity); }
        }
        private void checkInteger() throws DBException {
            long intCheck = Long.parseLong(entity);
            if (intCheck > Integer.MAX_VALUE || intCheck < Integer.MIN_VALUE) {
                throw new DBException.InvalidInteger(entity); }
        }
        private final int intLiteral;
        @Override public void sendCondition (DBCondition condition) { condition.addFloat((float) intLiteral); }
    }

    static class DBVString extends DBValue {
        public DBVString (DBToken token) throws DBException {
            super(new DBToken(token.getTokenType(), stripQuotes(token.getTokenBody())));
            checkBadString();
        }
        private void checkBadString() throws DBException {
            String[] badStrings = {"\t", "\n", "\f", "\r", "\"", "|", "£"};
            for (String s : badStrings) {
                if (entity.contains(s)) { throw new DBException.InvalidString(entity); }
            }
            if (entity.matches(".*\\d.*")) { throw new DBException.InvalidString(entity); }
        }
        static private String stripQuotes (String body) {
            String output;
            if (body.charAt(0) != '\'' || body.charAt(body.length() - 1) != '\'') { output = body; }
            else { output = body.split("'")[1]; }
            return output;
        }
        @Override public void sendCondition (DBCondition condition) { condition.addString(entity); }
    }
}
