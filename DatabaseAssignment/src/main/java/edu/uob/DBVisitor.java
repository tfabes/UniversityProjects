package edu.uob;

public interface DBVisitor {
    void visitCondition (DBCondition condition) throws DBException;
    void visitAttList (DBWildAttList.DBAttList attList);
    void visitValueList (DBList.DBValueList valueList);
    void visitNameValuePair (DBList.DBNameValuePair nameValuePair);
    void visitNameValueList (DBList.DBNameValueList nameValueList);
    void visitUseCMND (DBCommand.DBUse use);
    void visitCreateCMND (DBCommand.DBCreate create);
    void visitDropCMND (DBCommand.DBDrop drop);
    void visitAlterCMND (DBCommand.DBAlter alter);
    void visitInsertCMND (DBCommand.DBInsert insert);
    void visitUpdateCMND (DBCommand.DBUpdate update) throws DBException;
    void visitDeleteCMND (DBCommand.DBDelete delete);
    void visitJoinCMND (DBCommand.DBJoin join);
    void visitSelectCMND (DBCommand.DBSelect select);
}
