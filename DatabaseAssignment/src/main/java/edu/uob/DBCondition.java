package edu.uob;

import java.util.ArrayList;

class DBCondition extends DBExpression {
    final protected String operator;
    final protected DBExpression leftOperand;
    protected DBValue leftValue;
    final protected DBExpression rightOperand;
    final private ArrayList<String> stringList;
    final private ArrayList<Float> floatList;
    final private ArrayList<Boolean> boolList;
    private ETypes type;

    public DBCondition(DBToken operator, DBExpression opLeft, DBExpression opRight) {
        this.operator = operator.getTokenBody();
        leftOperand = opLeft;
        rightOperand = opRight;
        floatList = new ArrayList<>();
        stringList = new ArrayList<>();
        boolList = new ArrayList<>();
    }
    @Override void accept(DBVisitor visitor) throws DBException { visitor.visitCondition(this); }

    public void getValues() throws DBException {
        clearLists();
        leftValue.sendCondition(this);
        rightOperand.sendCondition(this);
    }

    public void addString(String value) { stringList.add(value); }
    public void addFloat(Float value) { floatList.add(value); }
    public void addBool(Boolean value) { boolList.add(value); }
    public void setOperand(DBValue entity) { leftValue = entity; }

    @Override
    public Boolean getEvaluation() throws DBException {
        if (evaluation == null) { evaluation = evaluateCondition(); }
        return super.getEvaluation();
    }

    public void clearLists() {
        stringList.clear();
        floatList.clear();
        boolList.clear();
    }

    @Override public void clearEvaluation() {
        evaluation = null;
        leftOperand.clearEvaluation();
        rightOperand.clearEvaluation();
    }

    @Override public void sendCondition (DBCondition condition) throws DBException {
        evaluation = evaluateCondition();
        condition.addBool(evaluateCondition());
    }

    public Boolean evaluateCondition() throws DBException {
        Boolean result;
        if (isOuter()) {
            result = outerCondition(operator.equalsIgnoreCase("AND")); }
        else {
            getValues();
            setComparisonType();
            switch (type) {
                case FLOAT -> result = evaluateFloat();
                case STRING -> result = evaluateString();
                default -> result = evaluateBool();
            }
        }
        return result;
    }

    private void setComparisonType() throws DBException {
        if (isFloat()) { type = ETypes.FLOAT; }
        else if (isString()) { type = ETypes.STRING; }
        else if (isBool()) { type = ETypes.BOOL; }
        else throw new DBException("[ERROR] Invalid comparison type");
    }

    private boolean isOuter() { return operator.equalsIgnoreCase("AND") ||
            operator.equalsIgnoreCase("OR"); }
    private boolean isFloat() { return floatList.size() == 2 ; }
    private boolean isString() { return stringList.size() == 2 ; }
    private boolean isBool() { return boolList.size() == 2 ; }

    private Boolean outerCondition(boolean isAND) throws DBException {
        boolean output;
        if (isAND) {
            if (!leftOperand.getEvaluation()) { output = false; }
            else { output = rightOperand.getEvaluation(); }
        }
        else { output = leftOperand.getEvaluation() || rightOperand.getEvaluation(); }
        return output;
    }

    private Boolean evaluateFloat() {
        float left = floatList.get(0);
        float right = floatList.get(1);
        switch (operator) {
            case "==" -> evaluation = equals(left,right);
            case ">" -> evaluation = moreThan(left,right);
            case "<" -> evaluation = lessThan(left,right);
            case ">=" -> evaluation = moreEquals(left,right);
            case "<=" -> evaluation = lessEquals(left,right);
            case "!=" -> evaluation = notEquals(left,right);
            case "LIKE" -> evaluation = like(left,right);
        }
        return evaluation;
    }

    private boolean equals(float left, float right) { return left == right; }
    private boolean moreThan(float left, float right) { return left > right; }
    private boolean lessThan(float left, float right) { return left < right; }
    private boolean moreEquals(float left, float right) { return left >= right; }
    private boolean lessEquals(float left, float right) { return left <= right; }
    private boolean notEquals(float left, float right) { return left != right; }
    private boolean like(float left, float right) { return Float.toString(left).contains(Float.toString(right)); }

    private Boolean evaluateString() throws DBException {
        String strLeft = stringList.get(0);
        String strRight = stringList.get(1);
        if (strLeft.equalsIgnoreCase("NULL") || strRight.equalsIgnoreCase("NULL")) {
            evaluation = evaluateNull(strLeft, strRight);
        } else { evaluation = stringComparisons(strLeft, strRight); }
        return evaluation;
    }

    private Boolean evaluateNull(String strLeft, String strRight) throws DBException {
        switch (operator) {
            case "==" -> evaluation = strLeft.equalsIgnoreCase(strRight);
            case "!=" -> evaluation = !strLeft.equalsIgnoreCase(strRight);
            default -> throw new DBException("[ERROR] Invalid NULL comparison");
        }
        return evaluation;
    }

    private Boolean stringComparisons(String strLeft, String strRight) {
        switch (operator) {
            case "==" -> evaluation = strEquals(strLeft,strRight);
            case ">" -> evaluation = strMoreThan(strLeft,strRight);
            case "<" -> evaluation = strLessThan(strLeft,strRight);
            case ">=" -> evaluation = strMoreEquals(strLeft,strRight);
            case "<=" -> evaluation = strLessEquals(strLeft,strRight);
            case "!=" -> evaluation = strNotEquals(strLeft,strRight);
            case "LIKE" -> evaluation = strLike(strLeft,strRight);
        }
        return evaluation;
    }

    private boolean strEquals(String left, String right) { return left.equals(right); }
    private boolean strMoreThan(String left, String right) { return left.compareTo(right) > 0; }
    private boolean strLessThan(String left, String right) { return left.compareTo(right) < 0; }
    private boolean strMoreEquals(String left, String right) { return left.compareTo(right) >= 0; }
    private boolean strLessEquals(String left, String right) { return left.compareTo(right) <= 0; }
    private boolean strNotEquals(String left, String right) { return !strEquals(left, right); }
    private boolean strLike(String left, String right) { return left.contains(right); }

    private Boolean evaluateBool() throws DBException {
        boolean lBool = boolList.get(0);
        boolean rBool = boolList.get(1);
        switch (operator) {
            case "==" -> evaluation = boolEquals(lBool,rBool);
            case "!=" -> evaluation = !boolEquals(lBool,rBool);
            default -> throw new DBException("[ERROR] Invalid comparison type");
        }
        return evaluation;
    }

    private boolean boolEquals(boolean left, boolean right) { return left == right; }

}