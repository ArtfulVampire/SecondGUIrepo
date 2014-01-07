//namespace coords
//{
//    const int x[] = {400, 1000, 100, 400, 700, 1000, 1300, 100, 400, 700, 1000, 1300, 100, 400, 700, 1000, 1300, 400, 1000};
//    const int y[] = {300, 300, 600, 600, 600, 600, 600, 900, 900, 900, 900, 900, 1200, 1200, 1200, 1200, 1200, 1500, 1500};
//    const char lbl[][19] = {"Fp1", "Fp2", "F7", "F3", "Fz", "F4", "F8", "T3", "C3", "Cz", "C4", "T4", "T5", "P3", "Pz", "P4", "T6", "O1", "O2"};
//};

#pragma once
//relative coords
namespace coords
{
    const double scale = 250./1600.;
//    const double x[] = {0.25, 0.625, 0.0625, 0.25, 0.4375, 0.625, 0.8125, 0.0625, 0.25, 0.4375, 0.625, 0.8125, 0.0625, 0.25, 0.4375, 0.625, 0.8125, 0.25, 0.625};
    const double x[] = {0.21875, 0.59375, 0.03125, 0.21875, 0.40625, 0.59375, 0.78125, 0.03125, 0.21875, 0.40625, 0.59375, 0.78125, 0.03125, 0.21875, 0.40625, 0.59375, 0.78125, 0.21875, 0.59375}; //-0.5/16
    const double y[] = {0.1875, 0.1875, 0.375, 0.375, 0.375, 0.375, 0.375, 0.5625, 0.5625, 0.5625, 0.5625, 0.5625, 0.75, 0.75, 0.75, 0.75, 0.75, 0.9375, 0.9375};
    const char lbl[][19] = {"Fp1", "Fp2", "F7", "F3", "Fz", "F4", "F8", "T3", "C3", "Cz", "C4", "T4", "T5", "P3", "Pz", "P4", "T6", "O1", "O2"};
};

