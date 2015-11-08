CREATE TABLE DOUBLE_TBL(f1 double);
INSERT INTO DOUBLE_TBL(f1) VALUES (' ');
INSERT INTO DOUBLE_TBL(f1) VALUES ('\t');
INSERT INTO DOUBLE_TBL(f1) VALUES ('\n');
INSERT INTO DOUBLE_TBL(f1) VALUES ('  \t  \t\t \n ');
INSERT INTO DOUBLE_TBL(f1) VALUES ('');
SELECT f1 FROM DOUBLE_TBL;


CREATE TABLE FLOAT_TBL(f1 float);
INSERT INTO FLOAT_TBL(f1) VALUES (' ');
INSERT INTO FLOAT_TBL(f1) VALUES ('\t');
INSERT INTO FLOAT_TBL(f1) VALUES ('\n');
INSERT INTO FLOAT_TBL(f1) VALUES ('  \t  \t\t \n ');
INSERT INTO FLOAT_TBL(f1) VALUES ('');
SELECT f1 FROM FLOAT_TBL;

DROP TABLE DOUBLE_TBL;
DROP TABLE FLOAT_TBL;
