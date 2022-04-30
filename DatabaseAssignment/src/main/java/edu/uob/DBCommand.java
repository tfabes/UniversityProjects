package edu.uob;

abstract class DBCommand {

    DBValue.DBVId nameID;
    protected DBCommand (DBValue.DBVId nameID) { this.nameID = nameID; }
    abstract void accept(DBVisitor visitor) throws DBException ;

    static class DBUse extends DBCommand {
        public DBUse (DBValue.DBVId dbName) { super(dbName); }
        @Override void accept(DBVisitor visitor) { visitor.visitUseCMND(this); }
    }

    static class DBCreate extends DBCommand {
        final String createType;
        final DBWildAttList.DBAttList attList;
        public DBCreate (DBValue.DBVId nameID, String createType, DBWildAttList.DBAttList attList) {
            super(nameID);
            this.createType = createType;
            this.attList = attList;
        }
        @Override void accept(DBVisitor visitor) { visitor.visitCreateCMND(this); }
    }

    static class DBDrop extends DBCommand {
        final String dropType;
        public DBDrop (DBValue.DBVId nameID, String dropType) {
            super(nameID);
            this.dropType = dropType;
        }
        @Override void accept(DBVisitor visitor) { visitor.visitDropCMND(this); }
    }

    static class DBAlter extends DBCommand {
        final String alterType;
        final DBValue.DBVId attName;
        public DBAlter (DBValue.DBVId nameID, String alterType, DBValue.DBVId attName) {
            super(nameID);
            this.alterType = alterType;
            this.attName = attName;
        }
        @Override void accept(DBVisitor visitor) { visitor.visitAlterCMND(this); }
    }

    static class DBInsert extends DBCommand {
        final DBList.DBValueList valueList;
        public DBInsert (DBValue.DBVId nameID, DBList.DBValueList valueList) {
            super(nameID);
            this.valueList = valueList;
        }
        @Override void accept(DBVisitor visitor) { visitor.visitInsertCMND(this); }
    }

    static class DBUpdate extends DBCommand {
        final DBList.DBNameValueList nameValueList;
        final DBCondition condition;
        public DBUpdate (DBValue.DBVId nameID, DBList.DBNameValueList nmVList, DBCondition cnd) {
            super(nameID);
            nameValueList = nmVList;
            condition = cnd;
        }
        @Override void accept(DBVisitor visitor) throws DBException { visitor.visitUpdateCMND(this); }
    }

    static class DBDelete extends DBCommand {
        final DBCondition condition;
        public DBDelete (DBValue.DBVId nameID, DBCondition cnd) {
            super(nameID);
            condition = cnd;
        }
        @Override void accept(DBVisitor visitor) { visitor.visitDeleteCMND(this); }
    }

    static class DBJoin extends DBCommand {
        final DBValue.DBVId secondTableID;
        final DBValue.DBVId firstAttName;
        final DBValue.DBVId secondAttName;
        public DBJoin (DBValue.DBVId[] idArray) {
            super(idArray[0]);
            secondTableID = idArray[1];
            firstAttName = idArray[2];
            secondAttName = idArray[3];
        }
        @Override void accept(DBVisitor visitor) { visitor.visitJoinCMND(this); }
    }

    static class DBSelect extends DBCommand {
        final DBWildAttList wildAttList;
        final DBCondition condition;
        public DBSelect (DBValue.DBVId nameID, DBWildAttList wildAL, DBCondition cnd) {
            super(nameID);
            wildAttList = wildAL;
            condition = cnd;
        }
        public boolean isConditionNull() { return condition == null; }
        @Override void accept(DBVisitor visitor) { visitor.visitSelectCMND(this); }
    }


}
