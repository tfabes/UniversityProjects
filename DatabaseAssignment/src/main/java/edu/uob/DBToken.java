package edu.uob;

public class DBToken {

    private final ETokenType tokenType;
    private final String tokenBody;

    public DBToken (ETokenType type, String body) {
        tokenType = type;
        tokenBody = body;
    }

    public ETokenType getTokenType() { return tokenType; }

    public String getTokenBody() { return tokenBody; }
}
