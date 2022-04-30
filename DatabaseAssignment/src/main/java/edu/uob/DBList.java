package edu.uob;

import java.util.ArrayList;

abstract class DBList extends DBExpression {

    static class DBValueList extends DBList {
        final private ArrayList<DBValue> valueList;
        public DBValueList (ArrayList<DBToken> tokenList) throws DBException {
            valueList = new ArrayList<>();
            for (DBToken token : tokenList) { valueList.add(getValueAsType(token)); }
        }
        public ArrayList<DBValue> getValueList() { return valueList; }
        @Override void accept(DBVisitor visitor) {
            visitor.visitValueList(this);
        }
    }

    static class DBNameValuePair extends DBList {
        final private DBValue.DBVId attName;
        final private DBValue value;
        public DBNameValuePair (DBToken token, DBToken nextToken) throws DBException {
            attName = new DBValue.DBVId(token);
            value = getValueAsType(nextToken);
        }
        public String getID() { return attName.entity; }
        public DBValue getValue() { return value; }
        @Override void accept(DBVisitor visitor) {
            visitor.visitNameValuePair(this);
        }
    }

    static class DBNameValueList extends DBList {
        final private ArrayList<DBNameValuePair> nmVList;
        public DBNameValueList (ArrayList<DBNameValuePair> pairList) {
            nmVList = new ArrayList<>();
            nmVList.addAll(pairList);
        }
        public ArrayList<DBNameValuePair> getNmVList() { return nmVList; }
        @Override void accept(DBVisitor visitor) { visitor.visitNameValueList(this); }
    }
}
