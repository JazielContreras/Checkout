#include <stdio.h>
#include <stdlib.h>


struct Grocery_Item {
    char groc_name[30];
    double groc_price;
    int quantity;
    struct Grocery_Item *next;
};

struct Customer {
    char name[30];
    double amount;
    struct Grocery_Item *groceries;
    struct Customer *next;
};


void getInput();
void putIn2DArray(char str[]);
void putInStruct(int counter, char fstring[counter][250]);
struct Grocery_Item* addGroceries(char itemName[], double price, int qty);
void addCustomer(char name[], double balance, struct Grocery_Item *ptr);
void displayCustomer();
void displayGroceries(struct Grocery_Item *temp);
struct Customer *pop();
void printToFile();


int main() {
    //gets input from input file and stores in linked lists
    getInput();
	//prints out the customers and their groceries
    displayCustomer();
    //prints the popped elements of the queue to output file
    printToFile();
    return 0;
}

void getInput() {
    FILE *fp;
    int num;
    char fstr[250];
    fp = fopen("input.txt", "r");
    if(fp == NULL) {
        printf("cannot open file\n");
        return;
    }
    while(fgets(fstr,250,fp)) {
        //stores line of text in fstr
        //printf("%s",fstr);
        putIn2DArray(fstr);
    }
    fclose(fp);
}

//creates 2D array to get rid of extraneous characters and helps index the elements that will later be added to the linked lists
void putIn2DArray(char str[]) {
    char fstr[100][250];
    int i, counter = 0,j = 0;
    //loops until character at i encounters '\0'
    for(i = 0; str[i] != '\0';i++) {
        //gets rid of unnecessary characters
        if((str[i] == '\"') || (str[i] == '{') || (str[i] == '}') || (str[i] == '[') || (str[i] == ']') || (str[i] == ' ') || (str[i] == '\n')) {
            continue;
        }
        //comma acts as delimiter
        else if(str[i] == ',') {
            //null character is added to the end of the string
            fstr[counter][j] = '\0';
            //counter is incremented to change word
            counter++;
            j = 0;
            continue;
        }

        else {
            //assigns necessary characters to fstr
            fstr[counter][j] = str[i];
            j++;
            //[[K,a,r,e,n],[1,5,.,0,0],[]]
        }
    }
    putInStruct(counter, fstr);
}

struct Grocery_Item *tail = NULL;
struct Grocery_Item *head = NULL;
struct Customer *queueHead = NULL;
struct Customer *queueTail = NULL;

//prints out the people within the linked list
void displayCustomer() {
    struct Customer *current = queueHead;
    //while current doesn't equal null it will continue
    while(current != NULL) {
        printf("Customer Name: %s\n", current->name);
        printf("Customer Amount: %lf\n",current->amount);
        printf("Groceries: \n");
        displayGroceries(current->groceries);
        printf("\n\n");
        current = current->next;
    }
}
//prints out the content of the grocery list
void displayGroceries(struct Grocery_Item *temp){
     struct Grocery_Item *list = temp;
     while(list != NULL) {
            printf("Grocery Item Name: %s\n",list->groc_name);
            printf("Grocery Item Price: %lf\n",list->groc_price);
            printf("Grocery Item Quantity: %d\n",list->quantity);
            list = list->next;
        }
}

//prints persons name with the amount they paid
//what they bought and what register they bought it from
void printToFile() {
    FILE *fp;
    int flag = 0;
    double multi = 0;
    double total = 0;
    int totalq = 0;
    fp = fopen("output.txt", "w");
    if(fp == NULL) {
        printf("cannot open file\n");
        return;
    }
    struct Customer *temp = pop();
    while(temp != NULL) {
      total = 0;
      totalq = 0;
      flag = 0;
        struct Grocery_Item *current = temp->groceries;
        while(current != NULL) {
            multi = (current->groc_price) * (current->quantity);
            total+= multi;
            totalq+= current->quantity;
            current = current->next;
        }

        current = temp->groceries;
        //if the amount owned by the person is less than their total than their purchases are rejected
        if((temp->amount) < total){
            flag = 1;
        }

        if(flag == 1) {
            fprintf(fp,"%s was rejected ",temp->name);
        }

        else if(flag == 0) {
          fprintf(fp,"%s paid $%.2lf for ",temp->name,total);
            while(current != NULL) {
                if(current->next == NULL) {
                    //prints out the quantity of a certain grocery item and it prints out the name of said item
                    fprintf(fp,"%d %s ",current->quantity,current->groc_name);
                }
                else
                    fprintf(fp,"%d %s, ",current->quantity,current->groc_name);
                current = current->next;
            }
        }

        //if the total quantity of items is greater than or equal to ten the they went to regular register
        if(totalq >= 10) {
            fprintf(fp,"at the regular register\n");
        }

        else if(totalq < 10) {
            fprintf(fp,"at the express register\n");
        }
        //count the total number of items
        //take items times their quantity and add up their sums to see if they can afford it
        //if they can't afford it then disregard their total and the items they got
        //check if they have 10 or more items. if they do then print out express register
        //else print out normal register
        temp = pop();
    }
    fclose(fp);
}

//gets rid of the first node in the Customer linked list
struct Customer *pop(){
    if (queueHead == NULL){
        printf("Underflow\n");
        return NULL;
    }
    else{
        struct Customer *temp = queueHead;
        queueHead = queueHead->next;
        return temp;
        //returns the first element of the linked list
    }
}

//puts input from file in to their corresponding data elements within the linked lists
void putInStruct(int counter, char fstring[counter][250]) {

    int i=0,j;
    struct Grocery_Item *ptr = NULL;
    float amount;
    double gprice;
    int quantity;
    //converts string to double to store in customer balance (cb)
    double cb = atof(fstring[1]);
    double x = 14;
    //printf("%d\n",counter);
    while(i <= counter) {
        if(i > 1) {
            int rem = i%3;
            if(rem == 2) {
                j = i;

                i++;
                continue;
                //put in grocery name;
            }
            else if(rem == 0) {
                gprice = atof(fstring[i]);

                i++;
                continue;
                //put in grocery price
            }
            else if (rem == 1) {
                //put in grocery quantity
                quantity = atof(fstring[i]);
                ptr = addGroceries(fstring[j], gprice, quantity);
                i++;
                continue;
            }

        }
        else{
            i++;
        }
    }

    addCustomer(fstring[0],cb,ptr);
    //resets the head and tail of the linked list to NULL
    head = tail = NULL;
}

//creates node for customer to be inserted into queue
void addCustomer(char name[], double balance, struct Grocery_Item *ptr){
    int i;
    struct Customer *temp = (struct Customer *)malloc(sizeof(struct Customer));
    //adds name of Customer character by character
    for(i = 0; name[i] != '\0';i++) {
        temp->name[i] = name[i];
    }
    temp->name[i] = '\0';
    //stores the balance in amount
    temp->amount = balance;
    //stores the ptr from the groceries list in the ptr groceries
    temp->groceries = ptr;

    if(queueHead == NULL) {
        queueHead = queueTail = temp;
    }

    else{
        queueTail->next = temp;
        queueTail = temp;
    }
}

//creates the customers grocery list which stores data from file
struct Grocery_Item* addGroceries(char itemName[], double price, int qty) {
    struct Grocery_Item *temp = (struct Grocery_Item *)malloc(sizeof(struct Grocery_Item));
    int i;
    //stores the name of the grocery item character by character
    for(i = 0; itemName[i] != '\0';i++) {
        temp->groc_name[i] = itemName[i];
    }
    temp->groc_name[i] = '\0';
    //store price in the price of grocery
    temp->groc_price = price;
    //stores the qty of items in quantity
    temp->quantity = qty;

    if(tail == NULL) {
        head = tail = temp;
    }

    else {
        tail->next = temp;
        tail = temp;
    }
    return head;
}





