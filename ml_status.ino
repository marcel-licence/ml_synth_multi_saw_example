void Status_ValueChangedFloat(const char *group, const char *descr, float value)
{
    Serial.printf("%s - %s: %0.3f\n", group, descr, value);
}

void Status_ValueChangedFloat(const char *descr, float value)
{
    Serial.printf("%s: %0.3f\n", descr, value);
}

void Status_ValueChangedInt(const char *group, const char *descr, int value)
{
    Serial.printf("%s - %s: %d\n", group, descr, value);
}

void Status_ValueChangedInt(const char *descr, int value)
{
    Serial.printf("%s: %d\n", descr, value);
}