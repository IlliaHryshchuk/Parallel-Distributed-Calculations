#include "Spreadsheet.h"



Spreadsheet::Spreadsheet(){}
Spreadsheet::~Spreadsheet(){}

int Spreadsheet::getRows()
{
	return  Spreadsheet::rows;
}
int Spreadsheet::getColumns()
{
	return  Spreadsheet::cols;
}

void Spreadsheet::resizeSpreadsheet(int numOfRows, int numOfCols)
{
	this->rows = numOfRows;
	this->cols = numOfCols;

	cell.resize(numOfRows);
	for (int i = 0; i < numOfRows; i++)
		cell[i].resize(numOfCols);
}

std::vector<std::string>& Spreadsheet::operator[](int index)
{
	return cell[index];
}

void Spreadsheet::printSpreadsheet()
{
	for (int i = 0; i < this->getRows(); i++)
	{
		std::cout << std::endl;
		for (int j = 0; j < this->getColumns(); j++)
			std::cout << this->cell[i][j] << "\t";
	}
	std::cout << std::endl;
}



std::string SpreadsheetGenerator::generateExpression()
{
	int numOfExpr, valA, valB;
	
	numOfExpr = rand() % 4 + 1;
	valA = rand() % 50 + 1;
	valB = rand() % 50 + 1;

	switch (numOfExpr)
	{
	case 1:
		return std::to_string(valA) + "+" + std::to_string(valB);
	case 2:
		return std::to_string(valA) + "-" + std::to_string(valB);
	case 3:
		return std::to_string(valA) + "*" + std::to_string(valB);
	case 4:
		return std::to_string(valA) + "/" + std::to_string(valB);
	}
}

void SpreadsheetGenerator::generate(const int numOfRows, const int numOfCols, Spreadsheet& spreadsheet)
{
	spreadsheet.resizeSpreadsheet(numOfRows, numOfCols);

#pragma omp parallel
	{
#pragma omp for 
		for (int i = 0; i < numOfRows; i++)
		{
			for (int j = 0; j < numOfCols; j++)
			{
				spreadsheet.cell[i][j] = this->generateExpression();
			}
		}
	}
}