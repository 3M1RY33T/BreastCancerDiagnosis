/*---------------------------------------------------
    Author: Yigit Yildiz
    Course: INFO3135
    Professor: Bestan Maaroof
    Assignment: Project 2 - Breast Cancer Diagnosis
*///-------------------------------------------------

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iomanip>

struct DatasetItem {
    std::string id;
    int clump_thickness;
    int uniformity_of_cell_size;
    int uniformity_of_cell_shape;
    int marginal_adhesion;
    int single_epithelial_cell_size;
    int bare_nuclei;
    int bland_chromatin;
    int normal_nucleoli;
    int mitoses;
    int Class;
    std::string result;
    bool isInvalid;
};

std::string GetExecutableDir() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    // Find the last backslash (the path separator in Windows)
    char* pos = strrchr(buffer, '\\');
    if (pos != nullptr) {
        *pos = '\0'; // Terminate the string at the last backslash
    }

    return std::string(buffer);
}

struct Node {
    std::string attribute; // The attribute to check at this node
    double threshold;      // The threshold value for comparison
    std::string result;    // The result if it's a leaf node ("Benign" or "Malignant")
    Node* left, * right;

    // Constructor for decision node
    Node(std::string attr, double thresh)
        : attribute(attr), threshold(thresh), left(nullptr), right(nullptr) {}

    // Constructor for leaf node
    Node(std::string res)
        : attribute(""), threshold(0), result(res), left(nullptr), right(nullptr) {}
};

const std::string UNIFORMITY_OF_CELL_SIZE = "Uniformity of Cell Size";
const std::string BARE_NUCLEI = "Bare Nuclei";
const std::string UNIFORMITY_OF_CELL_SHAPE = "Uniformity of Cell Shape";
const std::string BENIGN = "Benign";
const std::string CLUMP_THICKNESS = "Clump Thickness";
const std::string MALIGNANT = "Malignant";
const std::string BLAND_CHROMATIN = "Bland Chromatin";
const std::string MARGINAL_ADHESION = "Marginal Adhesion";

Node* build_tree() {
    Node* root = new Node(UNIFORMITY_OF_CELL_SIZE, 2);

    root->left = new Node(BARE_NUCLEI, 3);
    root->right = new Node(UNIFORMITY_OF_CELL_SHAPE, 2);

    //left side
    root->left->left = new Node(BENIGN);
    root->left->right = new Node(CLUMP_THICKNESS, 3);

    root->left->right->left = new Node(BENIGN);
    root->left->right->right = new Node(BLAND_CHROMATIN, 2);

    root->left->right->right->right = new Node(MALIGNANT);
    root->left->right->right->left = new Node(MARGINAL_ADHESION, 3);

    root->left->right->right->left->left = new Node(MALIGNANT);
    root->left->right->right->left->right = new Node(BENIGN);

    root->left->right->right->left->left = new Node(MALIGNANT);
    root->left->right->right->left->right = new Node(BENIGN);

    //right side
    root->right->left = new Node(CLUMP_THICKNESS, 5);
    root->right->right = new Node(UNIFORMITY_OF_CELL_SIZE, 4);

    root->right->left->left = new Node(BENIGN);
    root->right->left->right = new Node(MALIGNANT);

    root->right->right->left = new Node(BARE_NUCLEI, 2);
    root->right->right->right = new Node(MALIGNANT);

    root->right->right->left->left = new Node(MARGINAL_ADHESION, 3);
    root->right->right->left->right = new Node(CLUMP_THICKNESS, 6);

    root->right->right->left->left->left = new Node(BENIGN);
    root->right->right->left->left->right = new Node(MALIGNANT);

    root->right->right->left->right->left = new Node(UNIFORMITY_OF_CELL_SIZE, 3);
    root->right->right->left->right->right = new Node(MALIGNANT);

    root->right->right->left->right->left->left = new Node(MALIGNANT);
    root->right->right->left->right->left->right = new Node(MARGINAL_ADHESION, 5);

    root->right->right->left->right->left->right->left = new Node(BENIGN);
    root->right->right->left->right->left->right->right = new Node(MALIGNANT);

    return root;
}

std::string make_decision(Node* node, const std::map<std::string, double>& patientData) {
    while (node->left != nullptr && node->right != nullptr) { // While not a leaf node
        // Check the condition at this node with the patient's data
        if (patientData.at(node->attribute) <= node->threshold) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    return node->result; // Return the result at the leaf node
}

std::vector<std::string> split(const std::string& s, char delimeter) {
    std::vector<std::string> tokens;
    std::string t;
    std::istringstream tStream(s);
    while (std::getline(tStream, t, delimeter)) {
        tokens.push_back(t);
    }
    return tokens;
}

std::vector<DatasetItem> import_file(std::string filePath) {
    std::vector<DatasetItem> patientList;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Cannot open file." << std::endl;
        return patientList;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> tokens = split(line, ',');
        
        DatasetItem data = DatasetItem();
        data.id = tokens[0];
        for (std::string& t : tokens) {
            if (t == "?") {
                data.isInvalid = true;
                patientList.push_back(data);
                break;
            }
        }
        if (data.isInvalid)
        {
            continue;
        }
        data.clump_thickness = std::stoi(tokens[1]);
        data.uniformity_of_cell_size = std::stoi(tokens[2]);
        data.uniformity_of_cell_shape = std::stoi(tokens[3]);
        data.marginal_adhesion = std::stoi(tokens[4]);
        data.single_epithelial_cell_size = std::stoi(tokens[5]);
        data.bare_nuclei = std::stoi(tokens[6]);
        data.bland_chromatin = std::stoi(tokens[7]);
        data.normal_nucleoli = std::stoi(tokens[8]);
        data.mitoses = std::stoi(tokens[9]);
        data.Class = std::stoi(tokens[10]);

        patientList.push_back(data);
    }
    file.close();
    return patientList;
}

int main() {
    std::map<std::string, double> patientData;
    std::string filePath = GetExecutableDir() + "\\..\\..\\unformatted_data_v1.0.0.csv";

    std::ofstream outputFile(GetExecutableDir() + "\\..\\..\\results.csv", std::ofstream::out);

    if (!outputFile.is_open()) {
        std::cerr << "Could not create the output file" << std::endl;
        return EXIT_FAILURE;
    }
    
    int total_patients_processed = 0;
    int total_benign = 0;
    int total_malignant = 0;
    int total_invalid_patients = 0;

    std::vector<DatasetItem> dataItems = import_file(filePath);
    for (size_t i = 0; i < dataItems.size(); i++)
    {
        if (dataItems[i].isInvalid)
        {
            std::cout << "Patient Id " << std::setw(10) << "#" + dataItems[i].id << " Diagnosis: Data Invalid " << std::endl;
            total_invalid_patients++;
            continue;
        }
        patientData[UNIFORMITY_OF_CELL_SIZE] = dataItems[i].uniformity_of_cell_size;
        patientData[BARE_NUCLEI] = dataItems[i].bare_nuclei;
        patientData[UNIFORMITY_OF_CELL_SHAPE] = dataItems[i].uniformity_of_cell_shape;
        patientData[CLUMP_THICKNESS] = dataItems[i].clump_thickness;
        patientData[BLAND_CHROMATIN] = dataItems[i].bland_chromatin;
        patientData[MARGINAL_ADHESION] = dataItems[i].marginal_adhesion;

        Node* decisionTree = build_tree();
        std::string result = make_decision(decisionTree, patientData);
        dataItems[i].result = result;
        total_patients_processed++;

        if (dataItems[i].result == BENIGN) {
            total_benign++;
            dataItems[i].Class = 2;
        }
        else {
            total_malignant++;
            dataItems[i].Class = 4;
        }
        std::cout << "Patient Id " << std::setw(10) << "#"+ dataItems[i].id << " Diagnosis: " << result << std::endl;

        outputFile << dataItems[i].id << ",";
        outputFile << dataItems[i].clump_thickness << ",";
        outputFile << dataItems[i].uniformity_of_cell_size << ",";
        outputFile << dataItems[i].uniformity_of_cell_shape << ",";
        outputFile << dataItems[i].marginal_adhesion << ",";
        outputFile << dataItems[i].single_epithelial_cell_size << ",";
        outputFile << dataItems[i].bare_nuclei << ",";
        outputFile << dataItems[i].bland_chromatin << ",";
        outputFile << dataItems[i].normal_nucleoli << ",";
        outputFile << dataItems[i].mitoses << ",";
        outputFile << dataItems[i].Class << std::endl;

        outputFile << "\n";
    }

    outputFile.close();
    
    std::cout << std::endl;
    std::cout << "Total Patients Processed: " << total_patients_processed << std::endl;
    std::cout << "Total Benign: " << total_benign << std::endl;
    std::cout << "Total Malignant: " << total_malignant << std::endl;
    std::cout << "Total Invalid Patients: " << total_invalid_patients << std::endl;

    // Clean up memory (not shown here for brevity)
    return 0;
}