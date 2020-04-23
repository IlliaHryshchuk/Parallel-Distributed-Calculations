#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <omp.h>


class SpreadsheetGenerator;
class Spreadsheet;


class SpreadsheetGenerator
{
private:
	std::string generateExpression();
public:
	void generate(const int numOfRows, const int numOfCols, Spreadsheet &spreadsheet);
};



class Spreadsheet : public SpreadsheetGenerator
{
private:
	int rows, cols;
	std::vector<std::vector<std::string>> cell;
public:
	Spreadsheet();
	~Spreadsheet();

	int getRows();
	int getColumns();

	std::vector<std::string>& operator[](int index);
	void resizeSpreadsheet(int numOfRows, int numOfCols);
	void printSpreadsheet();

	friend class SpreadsheetGenerator;
};