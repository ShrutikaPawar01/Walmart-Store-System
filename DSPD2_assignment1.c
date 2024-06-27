#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_COLS 100
#define MAX_COL_WIDTH 100
#define MAX_ROW_WIDTH 1000
#define MAX_AISLES 10

typedef struct Node
{
	char AisleName[50];
	int ItemId;
	char ItemName[50];
	int Quantity;
	int ThresholdQuantity;
	int ExpiryYear;
	int ExpiryMonth;
	int ExpiryDate;
	struct Node *next;
} Node;

typedef struct ItemNode
{
	char name[100];
	int id;
	int quantity;
	float price;
	int AisleNo;

	struct ItemNode *next;
} ItemNode;

typedef struct BillNode
{
	int billNumber;
	struct ItemNode *itemHead;
	struct BillNode *next;
} BillNode;

typedef struct FrequencyNode
{
	int id;
	char name[100];
	int frequency;
	struct FrequencyNode *next;
} FrequencyNode;
void mergeSort(Node **lptr);
void mergeSortAisles(Node **aisles, int numAisles);

Node *createNode(const char *AisleName, int ItemId, const char *ItemName, int Quantity, int ThresholdQuantity, int ExpiryYear, int ExpiryMonth, int ExpiryDate)
{
	Node *nptr = (Node *)malloc(sizeof(Node));
	if (nptr == NULL)
	{
		printf("Memory allocation failed.\n");
		return NULL;
	}
	strcpy(nptr->AisleName, AisleName);
	nptr->ItemId = ItemId;
	strcpy(nptr->ItemName, ItemName);
	nptr->Quantity = Quantity;
	nptr->ThresholdQuantity = ThresholdQuantity;
	nptr->ExpiryYear = ExpiryYear;
	nptr->ExpiryMonth = ExpiryMonth;
	nptr->ExpiryDate = ExpiryDate;
	nptr->next = NULL;

	return nptr;
}

Node *insertAtStart(Node *lptr, const char *AisleName, int ItemId, const char *ItemName, int Quantity, int ThresholdQuantity, int ExpiryYear, int ExpiryMonth, int ExpiryDate)
{
	Node *nptr = createNode(AisleName, ItemId, ItemName, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate);
	if (nptr == NULL)
	{
		printf("Error creating node.\n");
		return lptr;
	}
	nptr->next = lptr;
	return nptr;
}

void display(Node *lptr)
{
	Node *current = lptr;
	while (current != NULL)
	{
		printf(" ItemId:%d, ItemName:%s, Quantity:%d, ThresholdQuantity:%d, ExpiryYear:%d, ExpiryMonth:%d, ExpiryDate:%d\n",
		       current->ItemId, current->ItemName, current->Quantity, current->ThresholdQuantity,
		       current->ExpiryYear, current->ExpiryMonth, current->ExpiryDate);
		current = current->next;
	}
}

Node *freeList(Node *lptr)
{
	Node *current = lptr;
	while (current != NULL)
	{
		Node *temp = current;
		current = current->next;
		free(temp);
	}
	return NULL;
}

Node *findItem(Node *lptr, int ItemId)
{
	Node *current = lptr;
	while (current != NULL)
	{
		if (current->ItemId == ItemId)
		{
			return current;
		}
		current = current->next;
	}
	return NULL;
}

Node **readDataFromFile(const char *filename, int *numAisles)
{
	FILE *file = fopen(filename, "r");
	if (!file)
	{
		printf("Error opening file.\n");
		return NULL;
	}

	char row[MAX_ROW_WIDTH];
	int maxAisleNo = 0;

	maxAisleNo = MAX_AISLES;
	*numAisles = maxAisleNo;

	Node **aisles = (Node **)malloc((maxAisleNo + 1) * sizeof(Node *));
	if (aisles == NULL)
	{
		printf("Memory allocation failed.\n");
		fclose(file);
		return NULL;
	}

	for (int i = 0; i <= maxAisleNo; i++)
	{
		aisles[i] = NULL;
	}

	rewind(file);

	while (fgets(row, sizeof(row), file))
	{
		int AisleNo, ItemId, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate;
		char ItemName[50];
		char AisleName[50];
		sscanf(row, "%d,%[^,],%d,%[^,],%d,%d,%d,%d,%d", &AisleNo, AisleName, &ItemId, ItemName, &Quantity, &ThresholdQuantity, &ExpiryYear, &ExpiryMonth, &ExpiryDate);
		if (AisleNo <= MAX_AISLES + 1)
			aisles[AisleNo - 1] = insertAtStart(aisles[AisleNo - 1], AisleName, ItemId, ItemName, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate);
	}

	fclose(file);

	return aisles;
}

Node **add(const char *filename, Node **Aisles, int numAisles)
{
	FILE *file = fopen(filename, "r");
	if (!file)
	{
		printf("Error opening file.\n");
		return Aisles;
	}

	char row[MAX_ROW_WIDTH];

	while (fgets(row, sizeof(row), file))
	{

		int AisleNo, ItemId, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate;
		char ItemName[50];
		char AisleName[50];
		if (sscanf(row, "%d,%[^,],%d,%[^,],%d,%d,%d,%d,%d", &AisleNo, AisleName, &ItemId, ItemName, &Quantity, &ThresholdQuantity, &ExpiryYear, &ExpiryMonth, &ExpiryDate) == 9)
		{
			if (AisleNo <= MAX_AISLES) // Checking if AisleNo is within the range
			{
				Node *existingItem = findItem(Aisles[AisleNo - 1], ItemId);
				if (existingItem != NULL)
				{
					existingItem->Quantity += Quantity;
				}
				else
				{
					Node *nptr = createNode(AisleName, ItemId, ItemName, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate);
					if (nptr == NULL)
					{
						printf("Error creating node.\n");
						continue;
					}
					Aisles[AisleNo - 1] = insertAtStart(Aisles[AisleNo - 1], AisleName, ItemId, ItemName, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate);
				}
			}
		}
	}

	fclose(file);
	mergeSortAisles(Aisles, numAisles);

	return Aisles;
}

Node **readAndAddNewAislesFromFile(const char *filename, Node **aisles, int *numAisles, int maxAisles)
{
	FILE *file = fopen(filename, "r");
	if (!file)
	{
		printf("Error opening file.\n");
		return aisles;
	}
	int flag = 0, i, n;
	for ((i = 1); i < MAX_AISLES && (flag == 0); i++)
	{
		if (aisles[i - 1] == NULL)
		{
			n = i;
			flag = 1;
		}
	}
	char row[MAX_ROW_WIDTH];
	fgets(row, sizeof(row), file);

	while (fgets(row, sizeof(row), file))
	{
		int AisleNo, ItemId, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate;
		char ItemName[50];
		char AisleName[50];
		if (sscanf(row, "%d,%[^,],%d,%[^,],%d,%d,%d,%d,%d", &AisleNo, AisleName, &ItemId, ItemName, &Quantity, &ThresholdQuantity, &ExpiryYear, &ExpiryMonth, &ExpiryDate) == 9)
		{
			aisles[n - 1] = insertAtStart(aisles[n - 1], AisleName, ItemId, ItemName, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate);
		}
	}
	mergeSort(&aisles[n - 1]);
	fclose(file);
	return aisles;
}
Node **Merge(const char *filename, Node **aisles, int *numAisles, int maxAisles, int from, int to)
{
	FILE *file = fopen(filename, "r");
	if (!file)
	{
		printf("Error opening file.\n");
		return aisles;
	}
	Node *ptr;
	ptr = aisles[to - 1];
	while (ptr->next != NULL)
	{
		ptr = ptr->next;
	}
	ptr->next = aisles[from - 1];
	aisles[from - 1] = NULL;

	char row[MAX_ROW_WIDTH];
	fgets(row, sizeof(row), file);
	while (fgets(row, sizeof(row), file))
	{
		int AisleNo, ItemId, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate;
		char ItemName[50];
		char AisleName[50];
		if (sscanf(row, "%d,%[^,],%d,%[^,],%d,%d,%d,%d,%d", &AisleNo, AisleName, &ItemId, ItemName, &Quantity, &ThresholdQuantity, &ExpiryYear, &ExpiryMonth, &ExpiryDate) == 9)
		{
			aisles[from - 1] = insertAtStart(aisles[from - 1], AisleName, ItemId, ItemName, Quantity, ThresholdQuantity, ExpiryYear, ExpiryMonth, ExpiryDate);
		}
	}
	mergeSortAisles(aisles, *numAisles);
	return aisles;
}

void Delete(Node **Aisles, int ItemId, int AisleNo)
{
	Node *ptr, *prev;
	prev = NULL;
	ptr = Aisles[AisleNo - 1];

	if (ptr == NULL)
	{
		printf("Aisle is already empty\n");
		return;
	}

	if (ptr->ItemId == ItemId)
	{
		Aisles[AisleNo - 1] = ptr->next;
		free(ptr);
		return;
	}

	while (ptr != NULL && ptr->ItemId < ItemId)
	{
		prev = ptr;
		ptr = ptr->next;
	}

	if (ptr == NULL || ptr->ItemId != ItemId)
	{
		printf("Item not found\n");
		return;
	}
	printf("%s item is deleted", ptr->ItemName);
	prev->next = ptr->next;
	free(ptr);
}

Node *merge(Node *lptr1, Node *lptr2)
{
	if (lptr1 == NULL)
		return lptr2;
	if (lptr2 == NULL)
		return lptr1;

	Node *mergedList;

	if (lptr1->ItemId < lptr2->ItemId)
	{
		mergedList = lptr1;
		mergedList->next = merge(lptr1->next, lptr2);
	}
	else
	{
		mergedList = lptr2;
		mergedList->next = merge(lptr1, lptr2->next);
	}

	return mergedList;
}

void partition(Node *lptr, Node **lptr1, Node **lptr2)
{
	Node *slow = lptr;
	Node *fast = lptr->next;

	while (fast != NULL)
	{
		fast = fast->next;
		if (fast != NULL)
		{
			slow = slow->next;
			fast = fast->next;
		}
	}

	*lptr1 = lptr;
	*lptr2 = slow->next;
	slow->next = NULL;
}

void mergeSort(Node **lptr)
{
	Node *head = *lptr;
	Node *lptr1;
	Node *lptr2;

	if (head == NULL || head->next == NULL)
	{
		return;
	}

	partition(head, &lptr1, &lptr2);

	mergeSort(&lptr1);
	mergeSort(&lptr2);

	*lptr = merge(lptr1, lptr2);
}

void mergeSortAisles(Node **aisles, int numAisles)
{
	for (int i = 0; i < numAisles; i++)
	{
		mergeSort(&aisles[i]);
	}
}

void copyDataToCSV(const char *filename, Node **aisles, int numAisles)
{
	FILE *file = fopen(filename, "w");
	if (!file)
	{
		printf("Error opening file for writing.\n");
		return;
	}

	fprintf(file, "AisleNo,AisleName,ItemId,ItemName,Quantity,ThresholdQuantity,ExpiryYear,ExpiryMonth,ExpiryDate\n");

	for (int i = 0; i <= numAisles; ++i)
	{
		Node *current = aisles[i];
		char name[50];

		while (current != NULL)
		{
			fprintf(file, "%d,%s,%d,%s,%d,%d,%d,%d,%d\n",
				i + 1, current->AisleName, current->ItemId, current->ItemName, current->Quantity, current->ThresholdQuantity,
				current->ExpiryYear, current->ExpiryMonth, current->ExpiryDate);
			current = current->next;
		}
	}

	fclose(file);
}

ItemNode *createItemNode(int id, char name[], int quantity, float price, int AisleNo)
{
	ItemNode *newNode = (ItemNode *)malloc(sizeof(ItemNode));
	if (newNode == NULL)
	{
		printf("Memory allocation failed\n");
		exit(1);
	}
	newNode->id = id;
	strcpy(newNode->name, name);
	newNode->quantity = quantity;
	newNode->price = price;
	newNode->AisleNo = AisleNo;
	newNode->next = NULL;
	return newNode;
}

BillNode *createBillNode(int billNumber)
{
	BillNode *newNode = (BillNode *)malloc(sizeof(BillNode));
	if (newNode == NULL)
	{
		printf("Memory allocation failed\n");
		exit(1);
	}
	newNode->billNumber = billNumber;
	newNode->itemHead = NULL;
	newNode->next = NULL;
	return newNode;
}

ItemNode *addItem(ItemNode *head, int id, char name[], int quantity, float price, int AisleNo)
{
	ItemNode *newNode = createItemNode(id, name, quantity, price, AisleNo);
	newNode->next = head;
	return newNode;
}

BillNode *addBill(BillNode *head, int billNumber)
{
	BillNode *newNode = createBillNode(billNumber);
	newNode->next = head;
	return newNode;
}

void displayBills(BillNode *head)
{
	BillNode *currentBill = head;
	while (currentBill != NULL)
	{
		printf("Bill Number: %d\n", currentBill->billNumber);
		printf("Items:\n");
		ItemNode *currentItem = currentBill->itemHead;
		while (currentItem != NULL)
		{
			printf("  ID: %d, Name: %s, Quantity: %d, Price: %.2f\n", currentItem->id, currentItem->name, currentItem->quantity, currentItem->price);
			currentItem = currentItem->next;
		}
		printf("\n");
		currentBill = currentBill->next;
	}
}

BillNode *readBillsFromFile(const char *filename)
{
	BillNode *billList = NULL;
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Error opening file\n");
		exit(1);
	}

	char line[1024];
	while (fgets(line, sizeof(line), file))
	{
		int billNumber, id, quantity, AisleNo;
		char name[100];
		float price;
		if (sscanf(line, "%d,%d,%[^,],%d,%f,%d", &billNumber, &id, name, &quantity, &price, &AisleNo) != 6)
		{

			continue;
		}
		BillNode *currentBill = billList;
		while (currentBill != NULL && currentBill->billNumber != billNumber)
		{
			currentBill = currentBill->next;
		}
		if (currentBill == NULL)
		{
			billList = addBill(billList, billNumber);
			currentBill = billList;
		}
		currentBill->itemHead = addItem(currentBill->itemHead, id, name, quantity, price, AisleNo); // Pass AisleNo to addItem function
	}

	fclose(file);
	return billList;
}

FrequencyNode *createFrequencyNode(int id, char name[])
{
	FrequencyNode *newNode = (FrequencyNode *)malloc(sizeof(FrequencyNode));
	if (newNode == NULL)
	{
		printf("Memory allocation failed\n");
		exit(1);
	}
	newNode->id = id;
	strcpy(newNode->name, name);
	newNode->frequency = 1;
	newNode->next = NULL;
	return newNode;
}

FrequencyNode *addFrequency(FrequencyNode *head, int id, char name[])
{
	FrequencyNode *current = head;
	while (current != NULL)
	{
		if (current->id == id)
		{
			current->frequency++;
			return head;
		}
		current = current->next;
	}
	FrequencyNode *newNode = createFrequencyNode(id, name);
	newNode->next = head;
	return newNode;
}
Node **billing(Node **Aisle, int maxAisles, BillNode *billList)
{
	BillNode *currentBill = billList;

	while (currentBill != NULL)
	{
		ItemNode *currentItem = currentBill->itemHead;
		Node *ptr;

		while (currentItem != NULL)
		{
			int flag = 0;
			ptr = Aisle[currentItem->AisleNo - 1];
			while (ptr != NULL && flag == 0)
			{
				if (ptr->ItemId == currentItem->id)
				{
					ptr->Quantity = ptr->Quantity - currentItem->quantity;
					flag = 1;
				}
				ptr = ptr->next;
			}
			currentItem = currentItem->next;
		}

		currentBill = currentBill->next;
	}
	return Aisle;
}

void findItemsOftenBought(BillNode *billList, int targetItemID)
{
	BillNode *currentBill = billList;
	FrequencyNode *itemFrequency = NULL;
	int maxFrequency = 0;
	char name[50];
	int flag = 0;

	while (currentBill != NULL)
	{
		ItemNode *currentItem = currentBill->itemHead;
		ItemNode *ptr = currentItem;
		int flag = 0;
		int targetAisleNo;

		while (ptr != NULL && flag == 0)
		{
			if (ptr->id == targetItemID)
			{
				flag = 1;
				targetAisleNo = ptr->AisleNo;
				strcpy(name, ptr->name);
			}
			ptr = ptr->next;
		}
		if (flag == 1)
		{
			while (currentItem != NULL)
			{
				if (currentItem->id != targetItemID && currentItem->AisleNo == targetAisleNo)
				{
					itemFrequency = addFrequency(itemFrequency, currentItem->id, currentItem->name);
				}

				currentItem = currentItem->next;
			}
		}
		currentBill = currentBill->next;
	}

	printf("Items often bought with item ID %d (%s):\n", targetItemID, name);
	FrequencyNode *currentFrequency = itemFrequency;
	while (currentFrequency != NULL)
	{
		if (currentFrequency->frequency >= 2)
		{
			printf("%d:%s\n", currentFrequency->id, currentFrequency->name);
			flag = 1;
		}

		currentFrequency = currentFrequency->next;
	}

	while (itemFrequency != NULL)
	{
		FrequencyNode *temp = itemFrequency;
		itemFrequency = itemFrequency->next;
		free(temp);
	}
	if (flag == 0)
	{
		printf("\nNo proper trend is observed or item is newly added");
	}
}
void Order(Node **aisles, int maxAisleNo)
{

	Node *ptr;
	printf("\nFollowing items needs to be ordered:\n");
	for (int i = 0; i < maxAisleNo; i++)
	{
		ptr = aisles[i];

		while (ptr != NULL)
		{

			if (ptr->ThresholdQuantity > ptr->Quantity)
			{
				printf("%s\n", ptr->ItemName);
			}
			ptr = ptr->next;
		}
	}
}
void Dairy_exp(Node **aisles, int maxAisleNo, int d, int m, int y)
{
	Node *ptr = NULL;
	ptr = aisles[0];
	int flag = 0;

	while (ptr != NULL)
	{

		if ((m == 12) && (d == 31))
		{
			if ((ptr->ExpiryYear == y + 1) && (ptr->ExpiryMonth == 1) && (ptr->ExpiryDate == 1))
			{

				printf("%s is about to expire.\n", ptr->ItemName);
				flag = 1;
			}
		}
		else if ((m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) && (d == 31))
		{
			if ((ptr->ExpiryYear == y) && (ptr->ExpiryMonth == m + 1) && (ptr->ExpiryDate == 1))
			{

				printf("%s is about to expire.\n", ptr->ItemName);
				flag = 1;
			}
		}
		else if ((m == 4 || m == 6 || m == 9 || m == 11) && (d == 30))
		{
			if ((ptr->ExpiryYear == y) && (ptr->ExpiryMonth == m + 1) && (ptr->ExpiryDate == 1))
			{

				printf("%s is about to expire.\n", ptr->ItemName);
				flag = 1;
			}
		}
		else if ((m == 2) && (((y % 4 != 0) && (d == 28)) || ((y % 4 == 0) && (d == 29))))
		{

			if ((ptr->ExpiryYear == y) && (ptr->ExpiryMonth == 3) && (ptr->ExpiryDate == 1))
			{

				printf("%s is about to expire.\n", ptr->ItemName);
				flag = 1;
			}
		}
		else
		{

			if ((ptr->ExpiryYear == y) && (ptr->ExpiryMonth == m) && (ptr->ExpiryDate - d <= 1) && (ptr->ExpiryDate - d >= 0))
			{

				printf("%s is about to expire.\n", ptr->ItemName);
				flag = 1;
			}
		}
		ptr = ptr->next;
	}
	if (flag == 0)
	{
		printf("Nothing is going to expire within a day");
	}
}
void ReadyToEat_exp(Node **aisles, int maxAisleNo, int d, int m, int y)
{
	Node *ptr = NULL;
	ptr = aisles[3];
	int days;
	int flag = 0;

	while (ptr != NULL)
	{
		days = 0;

		if (ptr->ExpiryYear == y)
		{
			if (ptr->ExpiryMonth == m)
			{

				days = ptr->ExpiryDate - d;
			}
			else
			{

				if ((m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) && (ptr->ExpiryMonth - m == 1))
				{
					days = 31 - d + ptr->ExpiryDate;
				}

				else if ((m == 4 || m == 6 || m == 9 || m == 11) && (ptr->ExpiryMonth - m == 1))
				{
					days = 30 - d + ptr->ExpiryDate;
				}
				else if ((m == 2) && (ptr->ExpiryMonth = 3))
				{
					if (y % 4 == 0)
					{
						days = 29 - d + ptr->ExpiryDate;
					}
					else
					{
						days = 28 - d + ptr->ExpiryDate;
					}
				}
			}
		}
		else if (ptr->ExpiryYear - y == 1)
		{

			if ((m == 12))
			{
				days = 31 - d + ptr->ExpiryDate;
			}
		}
		if (((days > 0) && (days <= 7)) || ((ptr->ExpiryYear == y) && (ptr->ExpiryMonth == m) && (ptr->ExpiryDate == d)))
		{
			printf("\n%s is about to expire within %d days", ptr->ItemName, days);
			flag = 1;
		}

		ptr = ptr->next;
	}
	if (flag == 0)
	{
		printf("nothing is going to expire within 7 days");
	}
}

void User_input(Node **aisles, int maxAisleNo)
{
	int id, qt, date, month, yr, flag = 0;
	int flag1 = 0;

	printf("\nEnter Item ID:");
	scanf("%d", &id);
	printf("Enter required quantity:");
	scanf("%d", &qt);
	while (flag1 == 0)
	{
		printf("Enter expiry in DDMMYYYY:\n");
		scanf("%d %d %d", &date, &month, &yr);
		if (date >= 1 && date <= 31 && month >= 1 && month <= 12 && yr >= 2000)
		{
			flag1 = 1;
		}
		else
		{
			printf("Invalid entry!!!");
		}
	}

	for (int i = 0; i < maxAisleNo && flag == 0; i++)
	{
		Node *ptr = aisles[i];

		while (ptr != NULL && ptr->ItemId < id)
		{
			ptr = ptr->next;
		}

		if (ptr != NULL && ptr->ItemId == id && ptr->Quantity >= qt &&
		    ptr->ExpiryYear >= yr && ptr->ExpiryMonth >= month && ptr->ExpiryDate >= date)
		{
			printf("Required item %s is available.\n", ptr->ItemName);
			flag = 1;
		}
	}

	if (flag == 0)
	{
		printf("Item not available.\n");
	}
}

int main()
{
	int numAisles = 0;
	int maxAisles = MAX_AISLES;
	int d_no, id, a_no;
	Node **aisles = readDataFromFile("walmartdata.csv", &numAisles);
	if (aisles != NULL)
	{
		mergeSortAisles(aisles, numAisles);
	}
	else
	{
		printf("Error reading data from file.\n");
	}

	int d, m, y;
	time_t t = time(NULL);
	struct tm *current_time = localtime(&t);
	d = current_time->tm_mday;
	m = current_time->tm_mon + 1;
	y = current_time->tm_year + 1900;

	printf("\nWelcome to Walmart store!\n");
	aisles = add("add_item.csv", aisles, numAisles);

	printf("\nThe required items are added in the file.\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	printf("\nEnter the details of the item to be deleted:\n");
	printf("Item ID:");
	scanf("%d", &id);
	printf("Aisle number:");
	scanf("%d", &a_no);
	Delete(aisles, id, a_no);
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	printf("\nIn order to check the availability of the required item:\n");
	User_input(aisles, maxAisles);
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	printf("\nThe following dairy items are about to expire:\n");
	Dairy_exp(aisles, maxAisles, d, m, y);
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	printf("\nThe following Ready to eat items are about to expire:\n");
	ReadyToEat_exp(aisles, maxAisles, d, m, y);
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	aisles = readAndAddNewAislesFromFile("Add_aisle.csv", aisles, &numAisles, maxAisles);
	printf("\nThe required aisle is added in the file.\n");
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	Order(aisles, maxAisles);
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	BillNode *billList = readBillsFromFile("bill.csv");

	findItemsOftenBought(billList, 103);
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");
	aisles = billing(aisles, maxAisles, billList);
	printf("\nStock of items is updated in walmart data file.");
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	int from, to;
	printf("Enter Aisle number of two aisles to be merged:");
	scanf("%d %d", &from, &to);
	aisles = Merge("medicine.csv", aisles, &numAisles, maxAisles, from, to);
	printf("\nAisles are succesfully merged");
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	printf("Enter Aisle number which is to be deleted: ");
	scanf("%d", &d_no);
	aisles[d_no - 1] = freeList(aisles[d_no - 1]);
	printf("Aisle %d is successfully deleted from the file.\n", d_no);
	printf("\n");
	printf("---------------------------------------------------------------------------------");
	printf("\n");

	copyDataToCSV("new.csv", aisles, numAisles);

	for (int i = 0; i <= numAisles; ++i)
	{
		freeList(aisles[i]);
	}
	free(aisles);

	return 0;
}