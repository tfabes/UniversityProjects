package edu.uob;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.regex.Pattern;

public class DBTokenizer {

    private final ArrayList<String> rawStringArray;
    private final ArrayList<DBToken> tokenArray;
    private int tokenIndex;

    private static final Pattern opRegex = Pattern.compile(
            "(?===)|(?<===)|(?=>)(?!=)|(?<=>)(?!=)|" +
            "(?=<)(?!=)|(?<=<)(?!=)|(?=>=)|(?<=>=)|" +
            "(?=<=)|(?<=<=)|(?=!=)|(?<=!=)|(?<==)(?<!==)(?!=)|(?==)(?<![=!<>])");

    private List<String> commandList;
    private List<String> structList;
    private List<String> keyWordList;
    private List<String> operatorList;
    private List<String> separatorList;

    public DBTokenizer (String inputMessage) throws DBException {
        rawStringArray = separateStrings(inputMessage);
        initTokenLists();
        tokenArray = createTokenArray(rawStringArray);
        if (!checkForEndSymbol()) { throw new DBException.TerminatorException(); }
        tokenIndex = 0;
    }

    public DBTokenizer() {
        initTokenLists();
        rawStringArray = new ArrayList<>();
        tokenArray = new ArrayList<>();
        tokenIndex = 0;
    }

    // Helper methods
    public DBToken getTokenThenIncrement() { return tokenArray.get(tokenIndex++); }

    public DBToken checkNextToken() { return tokenArray.get(tokenIndex); }

    public void ignoreToken() { tokenIndex++; }

    public int getNumberOfTokens() { return tokenArray.size(); }
    public boolean isLastToken() { return tokenIndex == lastTokenIndex(); }
    private int lastTokenIndex() { return getNumberOfTokens() - 1; }

    private boolean checkForEndSymbol() {
        return tokenArray.get(tokenArray.size()-1).getTokenBody().equals(";"); }

    // String separation methods
    private ArrayList<String> separateStrings (String inputMessage) {
        ArrayList<String> interimArr = new ArrayList<>(Arrays.asList(splitOnLiteralString(inputMessage)));
        interimArr = splitThenAddToArray(interimArr, "Operator");
        interimArr = splitThenAddToArray(interimArr, "KeyWords");
        interimArr = splitThenAddToArray(interimArr, "WhiteSpace");
        interimArr.removeIf(String::isBlank);
        return interimArr;
    }

    private ArrayList<String> splitThenAddToArray (ArrayList<String> inArr, String splitType) {
        ArrayList<String> outArr = new ArrayList<>();
        for (String s : inArr) {
            if (s.charAt(0) != '\'') {
                switch (splitType) {
                    case "Operator" -> outArr.addAll(Arrays.asList(splitOnOperators(s)));
                    case "KeyWords" -> outArr.addAll(Arrays.asList(splitOnKeyWords(s)));
                    case "WhiteSpace" -> outArr.addAll(Arrays.asList(splitOnWhiteSpace(s))); }
            } else { outArr.add(s); }
        }
        return outArr;
    }

    public String[] splitOnLiteralString (String message) {
        String[] strArr = message.split("(?=')|(?<=')");
        ArrayList<String> strList = new ArrayList<>();
        for (int i = 0; i < strArr.length; i++) {
            if (isQuote(strArr[i])) {
                if (isQuote(strArr[i+1])) { strList.add(strArr[i++]+strArr[i]); }
                else { strList.add(strArr[i++]+strArr[i++]+strArr[i]); }
            } else { strList.add(strArr[i]); }
        }
        strArr = new String[strList.size()];
        return strList.toArray(strArr);
    }

    private boolean isQuote(String str) { return str.charAt(0) == '\''; }

    public String[] splitOnOperators (String message) { return opRegex.split(message); }

    public String[] splitOnKeyWords (String message) { return message.split("(?=[(),;])|(?<=[(),;])"); }

    public String[] splitOnWhiteSpace (String message) { return message.split("\\s+"); }

    // Tokenization based on separated strings
    private void initTokenLists() {
        commandList = Arrays.asList("USE", "CREATE", "DROP", "ALTER", "INSERT",
                                    "SELECT", "UPDATE", "DELETE", "JOIN");
        structList = Arrays.asList("DATABASE", "TABLE");
        keyWordList = Arrays.asList(";", "INTO", "VALUES", "*", "FROM", "WHERE", "SET",
                                    "AND", "ON", "ADD");
        operatorList = Arrays.asList("=", "==", "<", ">", "<=", ">=", "!=", "LIKE");
        separatorList = Arrays.asList("(", ")", ",");
    }

    public ETokenType assignTokenType (String body) throws DBException {
        ETokenType type;
        if (commandList.contains(body.toUpperCase())) { type = ETokenType.COMMAND; }
        else if (structList.contains(body.toUpperCase())) { type = ETokenType.STRUCTURE; }
        else if (isPartOfSyntax(body)) { type = ETokenType.SYNTAX; }
        else if (isQuote(body)) { type = ETokenType.STRINGVALUE; }
        else if (body.matches("^[a-zA-Z0-9+\\-.]*$")) { type = ETokenType.ALPHANUMERIC; }
        else { throw new DBException.InvalidTokenException(body); }
        return type;
    }

    private boolean isPartOfSyntax(String body) {
        boolean returnValue = false;
        if (keyWordList.contains(body.toUpperCase())) { returnValue = true; }
        else if (operatorList.contains(body.toUpperCase())) { returnValue = true; }
        else if (separatorList.contains(body.toUpperCase())) { returnValue = true; }
        return returnValue;
    }

    private ArrayList<DBToken> createTokenArray (ArrayList<String> rawStrArr) throws DBException {
        ArrayList<DBToken> tokenArr = new ArrayList<>();
        for (String str : rawStrArr) {
            ETokenType type = assignTokenType(str);
            if (type == ETokenType.STRINGVALUE || type == ETokenType.ALPHANUMERIC) {
                tokenArr.add(new DBToken(type, str)); }
            else { tokenArr.add(new DBToken(type, str.toUpperCase())); }
        }
        return tokenArr;
    }

}
