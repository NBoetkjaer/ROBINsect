#pragma once

#include "StringNode.hpp" 
#include "NumericNode.hpp"

class Pos3D_32f_Node : public StringNode
{
public:
    Pos3D_32f_Node(const std::string &nodeName, float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : StringNode(nodeName, "[0,0,0]")
    {
        pNodeX = this->AddChild<FloatNode>("X", x);
        pNodeY = this->AddChild<FloatNode>("Y", y);
        pNodeZ = this->AddChild<FloatNode>("Z", z);
    }
    virtual ~Pos3D_32f_Node(){}
    
    void GetValue(std::string &strValue) const override 
    {
        std::string strVal;
        strValue = "[";
        pNodeX->GetValue(strVal);
        strValue += strVal + ", ";
        pNodeY->GetValue(strVal);
        strValue += strVal + ", ";
        pNodeZ->GetValue(strVal);
        strValue += strVal + "]";
    }

    void GetPosition(float& x, float& y, float &z) const 
    {
        x = pNodeX->Get();
        y = pNodeY->Get();
        z = pNodeZ->Get();
    }

    void SetPosition(float x, float y, float z)
    {
        pNodeX->Set(x);
        pNodeY->Set(y);
        pNodeZ->Set(z);
        if (pNodeX->IsValueChanged() || pNodeY->IsValueChanged() || pNodeZ->IsValueChanged())
        {
            SetAttributeChanged(valueAttrib.GetID());
        }
    }

    void SetValue(const char* pValues) override
    {
        static const char *pFormat = "[%g,%g,%g]";
        float values[3];
        const char *pChar = pValues;
        // Skip leading white spaces.
        while (*pChar != 0 && isspace((unsigned char)*pChar)) { ++pChar; }
        int numSuccess = sscanf(pChar, pFormat , &values[0], &values[1], &values[2]);
        if (numSuccess == 3)
        { 
            SetPosition(values[0], values[1], values[2]);
            value = pValues;
        }
        else
        {
            // ToDo: Report error (parsing string).
        }
    }

private:
    FloatNode* pNodeX;
    FloatNode* pNodeY;
    FloatNode* pNodeZ;
};

