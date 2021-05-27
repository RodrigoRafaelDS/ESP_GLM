/*
    This example opens Sqlite3 databases from SD Card and
    retrieves data from them.
    Before running please copy following files to SD Card:
    examples/sqlite3_sdmmc/data/mdr512.db
    examples/sqlite3_sdmmc/data/census2000names.db
    Connections:
     * SD Card | ESP32
     *  DAT2       -
     *  DAT3       SS (D5)
     *  CMD        MOSI (D23)
     *  VSS        GND
     *  VDD        3.3V
     *  CLK        SCK (D18)
     *  DAT0       MISO (D19)
     *  DAT1       -
*/

/*
Exemplos:
create table test (b1 blob);
insert into test values (shox96_0_2c('Hello World'));
insert into test values (shox96_0_2c('Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry''s standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.'));
select txt, length(txt) txt_len from (select shox96_0_2d(b1) txt from test);
select length(b1) compressed_len from test;
*/

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD.h"
#include <WiFi.h>


const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    Serial.printf("%s: ", (const char *)data);
    for (i = 0; i < argc; i++)
    {
        Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    Serial.printf("\n");
    return 0;
}

int openDb(const char *filename, sqlite3 **db)
{
    int rc = sqlite3_open(filename, db);
    if (rc)
    {
        Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    else
    {
        Serial.printf("Opened database successfully\n");
    }
    return rc;
}

char *zErrMsg = 0;
int db_exec(sqlite3 *db, const char *sql)
{
    Serial.println(sql);
    long start = micros();
    int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        Serial.printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        Serial.printf("Operation done successfully\n");
    }
    Serial.print(F("Time taken:"));
    Serial.println(micros() - start);
    return rc;
}

void setup()
{
    Serial.begin(115200);
    sqlite3 *db1;

    char *zErrMsg = 0;
    int rc;
    sqlite3_stmt *res;
    const char *tail;

    SPI.begin();
    SD.begin();

    sqlite3_initialize();

    // Open database 1
    if (openDb("/sd/census2000names.db", &db1))
        return;

    rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS test (Id  INTEGER  PRIMARY KEY,time INTEGER)");
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        return;
    }
    int temp = 888;
    char sql[100];
    sprintf(sql, "INSERT INTO test (time) VALUES (%Lu)", temp);
    // const char *sql = ("INSERT INTO test (time) VALUES (%Lu)",temp);
    // rc = sqlite3_prepare_v2(db1, sql, strlen(sql), &res, &tail);
    rc = db_exec(db1, sql);
    if (rc != SQLITE_OK)
    {
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(db1));
        sqlite3_close(db1);
        return;
    }

    // rc = db_exec(db1, "Select * from test where time = '10'");
    const char sql3[100]="Select * FROM test";
    rc = sqlite3_prepare_v2(db1, sql3, strlen(sql3) ,&res,&tail);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);

        return;
    }

    // Serial.printf("Year: %d\n", sqlite3_column_int(res, 0));
    int cont=0;
    while (sqlite3_step(res) == SQLITE_ROW)
    {
        cont++;
        Serial.printf("ID: %d\n", sqlite3_column_int(res, 0));
        Serial.printf("Value: %s\n", (const char *)sqlite3_column_text(res, 1));
        // Serial.printf("Name: %s\n", (const char *)sqlite3_column_text(res, 2));
        // Serial.printf("Total babies: %d\n", sqlite3_column_int(res, 3));
        // Serial.printf("Primary sex: %s\n", (const char *)sqlite3_column_text(res, 4));
        // Serial.printf("Ratio: %lf\n", sqlite3_column_double(res, 5));
        // Serial.printf("Per 100k in state: %lf\n\n", sqlite3_column_double(res, 6));
    }

    sqlite3_finalize(res);

    sqlite3_close(db1);
    // Serial.println(cont);
}

void loop()
{
}
