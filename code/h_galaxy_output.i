# 1 "./code/h_galaxy_output.h"
# 1 "/scratch-ssd/Workspace/Henriques2020_branch//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4

# 1 "/usr/include/stdc-predef.h" 3 4
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */




/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */

/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
# 52 "/usr/include/stdc-predef.h" 3 4
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */


/* We do not support C11 <threads.h>.  */
# 1 "<command-line>" 2
# 1 "./code/h_galaxy_output.h"

# 1 "./code/h_galaxy_output.h"
/*
 * Galaxy structure for output.
 *
 * NOTE: due to the way that the HDF5 builder routines work, variables should be commented out
 * with slash-star ... star-slash and not //.  Otherwise they are included in the properties list.
 *
 */
# 41 "./code/h_galaxy_output.h"
#pragma pack(1)
struct GALAXY_OUTPUT {
# 72 "./code/h_galaxy_output.h"
    int Type; // None //Galaxy type: 0 for central galaxies of a main halo, 1 for central galaxies in sub-halos, 2 for satellites without halo.

    int HaloIndex; // None // ?Unique ID of MPA halo containing this galaxy
# 85 "./code/h_galaxy_output.h"
    int SnapNum; // None //The snapshot number where this galaxy was identified.
    float LookBackTimeToSnap; // yr // The time from a given snapshot to z=0
    float CentralMvir; // 10^10/h Msun // virial mass of background (FOF) halo containing this galaxy
    float CentralRvir; // Mpc/h // Proper[?] R200 cf critical of background (FOF) halo containing this galaxy
    float DistanceToCentralGal[3]; // Mpc/h // Proper[?] components of the distance between this galaxy and the galaxy at the centre of the FoF group.
    float Pos[3]; // 1/h Mpc // Comoving galaxy/subhalo position
    float Vel[3]; // km/s // Galaxy/subhalo peculiar velocity
    int Len; // None // Number of particles in the associated subhalo  
    /* properties of subhalo at the last time this galaxy was a central galaxy */
    float Mvir; // 10^10/h Msun // M200 cf critical of the halo last time galaxy was type 0
    float Rvir; // Mpc/h // R200 cf critical of the halo last time galaxy was type 0
    float Vvir; // km/s // Virial velocity of the halo last time galaxy was type 0
    float Vmax; // km/s //Maximum rotational velocity of the subhalo, or the last value for type 2's galaxies.
    float HaloSpin[3]; // Mpc/h km/s // The specific angular momentum of the stellar disk




    float InfallVmax; // km/s // Maximum rotational velocity of the host halo of this galaxy at infall (ie last time a type 0)
    float InfallVmaxPeak; // km/s // ? Peak Vmax along past history
    int InfallSnap; // None // Most recent (largest) snapnum at which this galaxy's type changed from 0 to 1 or 2
    float InfallHotGas; // 10^10 Msun/h // Mass in hot gas at the time of infall (same as hotGas for type 0 galaxies).
    float HotRadius; // Mpc/h // Proper[?] radius out to which hot gas extends: rvir for type 0; 0 for type 2; maximum radius out to which hot gas is not stripped for type 1.
    /*dynamical friction merger time*/

    float OriMergTime; // yr // Estimated dynamical friction time when the merger clock is set.
    float MergTime; //yr // Estimated remaining merging time. 
# 124 "./code/h_galaxy_output.h"
    /* baryonic reservoirs */
    float ColdGas; // 10^10/h Msun // Mass in cold gas.

    float ColdGasRings[RNUM]; // 1e10 Msun/h //  Mass of clod gas in each annulur ring.
    float H2fraction; // None //  Fraction of ColdGas in the form of H_2
    float H2fractionRings[RNUM]; // None //  H2 fraction within each annular ring.

    float StellarMass; // 10^10/h Msun // Total mass in stars in the disk and the bulge combined
    float DiskMass; // 10^10/h Msun // Mass of stars in the disk
    float BulgeMass; // 10^10/h Msun // Mass of stars in the bulge

    float DiskMassRings[RNUM]; // 1e10 Msun/h // Mass of stars within each annular ring
    float BulgeMassRings[RNUM]; // 1e10 Msun/h // Mass of stars within each annular ring

    float HotGas; // 10^10/h Msun // Mass in hot gas
    /*float ReheatedGas; // 10^10/h Msun // Mass in reheated gas*/
    float EjectedMass; // 10^10/h Msun // Mass in ejected gas



    float BlackHoleMass; // 10^10/h Msun // Mass of central black hole
    /* float BlackHoleGas; // 10^10/h Msun // Mass in BH accretion disk */
    /* ICL magnitude and mass*/
    float ICM; //10^10/h Msun //Total mass in metals in intra-cluster stars, for type 0,1

    float MassFromInSitu; // 1e10 Msun/h // Mass formed in situ.
    float MassFromMergers; // 1e10 Msun/h // Mass accreted from mergers.
    float MassFromBursts; // 1e10 Msun/h // Mass formed in starbursts




    float MetalsColdGas[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in cold gas.

    float MetalsColdGasRings[RNUM][NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in cold gas in each annular ring

    float MetalsStellarMass[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in the disk
    float MetalsDiskMass[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in the disk
    float MetalsBulgeMass[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in the bulge

    float MetalsDiskMassRings[RNUM][NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in stars in each annular ring
    float MetalsBulgeMassRings[RNUM][NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in stars in each annular ring

    float MetalsHotGas[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in the hot gas
    /* float MetalsReheatedGas[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in the Reheated gas */
    float MetalsEjectedMass[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in the ejected gas



    float MetalsICM[NUM_METAL_CHANNELS]; // 10^10/h Msun // Mass in metals in intra-cluster stars, for type 0,1




    /* misc */
    float PrimordialAccretionRate; // Msun/yr // Accretion rate of primordial gas.
    float CoolingRadius; // Mpc/h // The radius within which the cooling time scale is shorter than the dynamical timescale
    /* float CoolingGas; // 10^10/h Msun // Mass of cooling gas */







    float CoolingRate; // Msun/yr // Cooling rate of the hot gas
    float CoolingRate_beforeAGN; // Msun/yr // What the cooling rate of the hot gas would have been if there was no AGN feedback.
    float QuasarAccretionRate; // Msun/yr // Rate at which cold gas is accreted into the central black hole in the quasar mode.
    float RadioAccretionRate; // Msun/yr // Rate at which hot gas is accreted into the central black hole in the radio mode.
    float Sfr; // Msun/yr // Star formation rate

    float SfrRings[RNUM]; // Msun/yr // Star formation rate within each annular ring

    float SfrBulge; // Msun/yr // Star formation rate in bulge.
    float XrayLum; // log10(erg/sec) // (log_10 of) X-Ray luminosity
    float BulgeSize; // Mpc/h // Half mass radius of bulge
    float DiskRadius; // Mpc/h // Size of the stellar disk, 3x the scale length.
    float ColdGasRadius; // Mpc/h // Size of the gas disk, 3x the scale length.
    float StellarHalfMassRadius; // Mpc/h // stellar Half mass radius

    float StellarHalfLightRadius; // Mpc/h // stellar Half light radius
    float CosInclination; // deg // Inclination of the galaxy. Derived from the angle between the stellar spins of the galaxy and the z-axis

    int DisruptOn; // None // 0: galaxy merged onto merger center 1: galaxy was disrupted before merging onto its descendant, matter went into ICM of merger center;

    int MergeOn; // None // 0: standard delucia-like merger behaviour for type 1 galaxy; 1: galaxy mass > halo mass, separate dynamical friction time calculated ....



       /* magnitudes in various bands */


    float MagDust[40]; // AB mag // dust corrected, rest-frame absolute mags
    float Mag[40]; // AB mag // rest-frame absolute mags
    float MagBulge[40]; // AB mag // rest-frame absolute mags for the bulge
# 251 "./code/h_galaxy_output.h"
    float MassWeightAge; //10^9yr //The age of this galaxy weighted by mass of its components.

    float rbandWeightAge; // 10^9yr // The age of this galaxy weighted by mass of its components.


    int sfh_ibin; // None // Index of highest star formation history bin currently in use
    int sfh_numbins; // None // Number of non empty star formation history bins

    /* float sfh_time[SFH_NBIN]; // yr // lookback time to middle of star formation history bin. */
    /* float sfh_dt[SFH_NBIN]; // yr // Width of star formation history bin. */
    float sfh_DiskMass[SFH_NBIN]; // 10^10 Msun/h // Star formation history in the disk.
    float sfh_BulgeMass[SFH_NBIN]; // 10^10 Msun/h // Star formation history in the bulge.

    float sfh_DiskMassRings[RNUM][SFH_NBIN]; // 10^10 Msun/h // Star formation history in the disk RINGS.
    float sfh_BulgeMassRings[RNUM][SFH_NBIN]; // 10^10 Msun/h // Star formation history in the bulge RINGS.

    float sfh_ICM[SFH_NBIN]; // 10^10 Msun/h // Star formation history in intra-cluster stars.
    float sfh_MetalsDiskMass[SFH_NBIN][NUM_METAL_CHANNELS]; // 10^10 Msun/h // Metal formation history in the disk.
    float sfh_MetalsBulgeMass[SFH_NBIN][NUM_METAL_CHANNELS]; // 10^10 Msun/h // Metal formation history in the bulge.
    float sfh_MetalsICM[SFH_NBIN][NUM_METAL_CHANNELS]; // 10^10 Msun/h // Metal formation history in the ICM.
# 283 "./code/h_galaxy_output.h"
    //All: [H][He][Cb][N][O][Ne][Mg][Si][S][Ca][Fe] or //Only [H][He][O][Mg][Fe]

    float sfh_DiskMass_elements[SFH_NBIN][NUM_ELEMENTS]; // 10^10 Msun/h // History of mass of elements locked up in stars in disk.
    float sfh_BulgeMass_elements[SFH_NBIN][NUM_ELEMENTS]; // 10^10 Msun/h // History of mass of elements locked up in stars in bulge.
    float sfh_ICM_elements[SFH_NBIN][NUM_ELEMENTS]; // 10^10 Msun/h // History of mass of elements locked up in stars in the ICM.

    float DiskMass_elements[NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in stars in disk.
    float BulgeMass_elements[NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in stars in bulge.

    float DiskMassRings_elements[RNUM][NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in stars in each annular ring.
    float BulgeMassRings_elements[RNUM][NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in stars in each annular ring.

    float ColdGas_elements[NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in stars in cold gas.

    float ColdGasRings_elements[RNUM][NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in cold gas in each annular ring.

    float HotGas_elements[NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in hot gas.
    /* float ReheatedGas_elements[NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in reheated gas. */
    float ICM_elements[NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in stars in the ICM
    float EjectedMass_elements[NUM_ELEMENTS]; // 10^10 Msun/h // Mass of elements locked up in ejected gas.




};

// next only of interest to DB output, which generally requires complete tree

struct SFH_BIN {
    long long GalID; // None // ID of the galaxy
    short snapnum; // None // snapnum of the galaxy, repeated here for faster lookups of times etc
    short sfh_ibin; // None //Index of highest bin currently in use
    /* float sfh_time; // yr // Lookback time at the middle of bin. */
    /* float sfh_dt; // yr // time width of bin. */
    float sfh_DiskMass; // 1e10 Msun/h // SFH of disk
    float sfh_BulgeMass; // 1e10 Msun/h // SFH of bulge

    float sfh_DiskMassRings[RNUM]; // 10^10 Msun/h // Star formation history in the disk RINGS.
    float sfh_BulgeMassRings[RNUM]; // 10^10 Msun/h // Star formation history in the bulge RINGS.

    float sfh_ICM; // 1e10 Msun/h // SFH of ICM
# 332 "./code/h_galaxy_output.h"
    float sfh_MetalsDiskMass[NUM_METAL_CHANNELS]; // 1e10 Msun/h // Metals locked up in stars in disk.
    float sfh_MetalsBulgeMass[NUM_METAL_CHANNELS]; // 1e10 Msun/h // Metals locked up in stars in bulge.
    float sfh_MetalsICM[NUM_METAL_CHANNELS]; // 1e10 Msun/h // Metals locked up in stars in ICM.
};

struct SFH_Time {
    int snapnum; // None // snapnum
    int bin; // None // index of current bin
    /* proposal: in output write the start of the bin and its end, rather than centre and dt */
    double lookbacktime; // yr // lookback time to centre of current bin
    double dt; // yr // width of the current bin
    int nbins; // None // number of highest resolution bins used to create current bin
};

#pragma pack()
