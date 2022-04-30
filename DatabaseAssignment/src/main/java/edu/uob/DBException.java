package edu.uob;

import java.io.Serial;

public class DBException extends Exception {
    @Serial
    private static final long serialVersionUID = -1279276819518833682L;

    public DBException(String message) { super(message); }

    public static class InvalidTokenException extends DBException {
        @Serial
        private static final long serialVersionUID = 5161557031804489627L;
        public InvalidTokenException (String body) {
            super("[ERROR] This word was not valid in your command: "+body); }
    }

    public static class TerminatorException extends DBException {
        @Serial
        private static final long serialVersionUID = 3158366355846741024L;
        public TerminatorException () {
            super("[ERROR] Statement terminator \";\" is missing"); }
    }

    public static class CommandException extends DBException {
        @Serial
        private static final long serialVersionUID = 8466769965011898875L;
        public CommandException (String cmnd) {
            super("[ERROR] Incorrect syntax for "+cmnd); }
    }

    public static class StructException extends DBException {
        @Serial
        private static final long serialVersionUID = -6767154386855789180L;
        public StructException (String cmnd) {
            super("[ERROR] "+cmnd+" can only be used on a DATABASE or TABLE"); }
    }

    public static class AlterException extends DBException {
        @Serial
        private static final long serialVersionUID = -8782374118666664879L;
        public AlterException() {
            super("[ERROR] ALTER command can only be used with ADD or DROP alteration types"); }
    }

    public static class IDException extends DBException {
        @Serial
        private static final long serialVersionUID = -2194208162099029995L;
        public IDException() { super("[ERROR] Invalid identifier - use numbers and letters only"); }
    }

    public static class InvalidValue extends DBException {
        @Serial
        private static final long serialVersionUID = -5671741690640043047L;
        public InvalidValue(String val) { super("[ERROR] Invalid value: "+val); }
    }

    public static class InvalidCondition extends DBException {
        @Serial
        private static final long serialVersionUID = -5863468199609106061L;
        public InvalidCondition() { super("[ERROR] Invalid syntax for Condition"); }
    }

    public static class InvalidString extends DBException {
        @Serial
        private static final long serialVersionUID = 6264958782725963545L;
        public InvalidString(String val) { super("[ERROR] Invalid string: "+val); }
    }

    public static class InvalidFloat extends DBException {
        @Serial
        private static final long serialVersionUID = 3689257553737560306L;
        public InvalidFloat(String val) { super("[ERROR] Invalid float: "+val); }
    }

    public static class InvalidInteger extends DBException {
        @Serial
        private static final long serialVersionUID = -8416531976699947552L;
        public InvalidInteger(String val) { super("[ERROR] Invalid integer:"+val); }
    }

    public static class InvalidName extends DBException {
        @Serial
        private static final long serialVersionUID = 5777276696598953323L;
        public InvalidName(String name) { super("[ERROR] Invalid name: "+name); }
    }

    public static class AlreadyExists extends DBException {
        @Serial
        private static final long serialVersionUID = 47098854009371474L;
        public AlreadyExists(String name, boolean isDB) {
            super("[ERROR] "+(isDB ? "Database '" : "Table '")+name+"' already exists"); }
    }

    public static class CreateFailed extends DBException {
        @Serial
        private static final long serialVersionUID = -2695589946052757872L;
        public CreateFailed(String name) { super("[ERROR] Could not create "+name); }
    }

    public static class DBNotUsed extends DBException {
        @Serial
        private static final long serialVersionUID = -2142366194551021740L;
        public DBNotUsed() { super("[ERROR] No database in use. Try again after 'USE <DB> ;'"); }
    }

    public static class AttNotFound extends DBException {
        @Serial
        private static final long serialVersionUID = -2338020348250332544L;
        public AttNotFound(String name) { super("[ERROR] Attribute "+name+" not found"); }
    }

}
