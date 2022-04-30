package edu.uob;

import java.util.ArrayList;

abstract class DBWildAttList extends DBList {

    abstract protected boolean isWild();

    static class DBWild extends DBWildAttList {
        final public String wild;
        public DBWild (DBToken token) { wild = token.getTokenBody(); }
        @Override void accept(DBVisitor visitor) {}
        @Override protected boolean isWild() { return true; }
    }

    static class DBAttList extends DBWildAttList {
        final public ArrayList<DBValue.DBVId> attNames;
        public DBAttList (ArrayList<DBToken> tokenList) {
            attNames = new ArrayList<>();
            for (DBToken token : tokenList) { attNames.add(new DBValue.DBVId(token)); }
        }
        @Override void accept(DBVisitor visitor) { visitor.visitAttList(this); }
        @Override protected boolean isWild() { return false; }
    }
}
