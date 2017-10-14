#pragma ident "$Id$"

/**
 * @file Variable.hpp
 * Class to define and handle 'descriptions' of GNSS variables.
 */

#ifndef GPSTK_VARIABLE_HPP
#define GPSTK_VARIABLE_HPP

//============================================================================
//
//  This file is part of ROCKET, the Real-time Orbit determination and ClocK
//  estimation Toolkit. The ROCKET software is based on some open-source
//  software, including GPSTK, GAMIT and RTKLIB and some other open-source
//  resources.
//
//  Until now, the Copyright follows the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 2.1 of the
//  License, or any later version.  You should have received a copy of the
//  GNU Lesser General Public License.
//
//  Copyright(c)
//
//  Shoujian Zhang, Wuhan University, 2014-2016
//
//============================================================================
//
//  2014/02/20      Add the new class "Coefficient" to store the coefficients
//                  for variables.
//
//============================================================================


#include "DataStructures.hpp"
#include "StochasticModel2.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /// Class to define and handle 'descriptions' of GNSS variables.
    class Variable
    {
    public:

        /// Default constructor for Variable
        Variable();


        /** Common constructor for Variable.
         *  By default, it is indexed by SourceID.
         *
         * @param type             TypeID of variable.
         * @param pModel           Pointer to StochasticModel associated with
         *                         this variable. By default, it is a white
         *                         noise model.
         * @param sourceIndexed    Whether this variable is SourceID-indexed
         *                         or not. By default, it IS SourceID-indexed.
         * @param satIndexed       Whether this variable is SatID-indexed
         *                         or not. By default, it is NOT SatID-indexed.
         * @param variance         Initial variance assigned to this variable.
         * @param coef             Default coefficient assigned.
         * @param forceCoef        Always use default coefficient.
         */
        Variable( const TypeID& type,
                  StochasticModel2* pModel = &Variable::defaultModel,
                  bool sourceIndexed       = true,
                  bool satIndexed          = false,
                  double variance          = 1.0e10,
                  double coef              = 1.0,
                  bool forceCoef           = false,
                  int  now_index           = -1,
                  int  pre_index           = -1 );


        /// Get variable type
        TypeID getType() const
        { return varType; };


        /** Set variable type
         *
         * @param type        New TypeID of variable.
         */
        Variable& setType(const TypeID& type)
        { varType = type; return (*this); };


        /// Get variable model pointer
        StochasticModel2* getModel() const
        { return pVarModel; };


        /** Set variable model pointer
         *
         * @param pModel      Pointer to StochasticModel associated with
         *                    this variable. By default, it is a white
         *                    noise model.
         */
        Variable& setModel(StochasticModel2* pModel)
        { pVarModel = pModel; return (*this); };


        /// Get if this variable is SourceID-indexed
        bool getSourceIndexed() const
        { return isSourceIndexed; };


        /** Set if this variable is SourceID-indexed.
         *
         * @param sourceIndexed    Whether this variable is SourceID-indexed
         *                         or not. By default, it IS SourceID-indexed.
         */
        Variable& setSourceIndexed(bool sourceIndexed)
        { isSourceIndexed = sourceIndexed; return (*this); };


        /// Get if this variable is SatID-indexed.
        bool getSatIndexed() const
        { return isSatIndexed; };


        /** Set if this variable is SatID-indexed.
         *
         * @param satIndexed       Whether this variable is SatID-indexed
         *                         or not. By default, it is NOT SatID-indexed.
         */
        Variable& setSatIndexed(bool satIndexed)
        { isSatIndexed = satIndexed; return (*this); };


        /// Get if this variable is Type-indexed.
        bool getTypeIndexed() const
        { return isTypeIndexed; };


        /** Set if this variable is Type-indexed.
         *
         * @param typeIndexed      Whether this variable is Type-indexed
         *                         or not. By default, it IS Type-indexed.
         */
        Variable& setTypeIndexed(bool typeIndexed)
        { isTypeIndexed = typeIndexed; return (*this); };


        /// Get value of initial variance assigned to this variable.
        double getInitialVariance() const
        { return initialVariance; };


        /** Set value of initial variance assigned to this variable.
         *
         * @param variance      Initial variance assigned to this variable.
         */
        Variable& setInitialVariance(double variance)
        { initialVariance = variance; return (*this); };


        /// Get value of default coefficient assigned to this variable.
        double getDefaultCoefficient() const
        { return defaultCoefficient; };


        /** Set value of default coefficient assigned to this variable.
         *
         * @param coef    Default coefficient assigned to this variable.
         */
        Variable& setDefaultCoefficient(double coef)
        { defaultCoefficient = coef; return (*this); };


        /// Ask if default coefficient will always be used.
        bool isDefaultForced() const
        { return forceDefault; };


        /** Set if default coefficient will always be used.
         *
         * @param forceCoef     Always use default coefficient.
         */
        Variable& setDefaultForced(bool forceCoef)
        { forceDefault = forceCoef; return (*this); };


        /// Get internal source this variable is assigned to (if any).
        SourceID getSource() const
        { return varSource; };


        /** Set internal source this variable is assigned to.
         *
         * @param source     Internal, specific SourceID of variable.
         */
        Variable& setSource(const SourceID& source)
        { varSource = source; return (*this); };


        /// Get internal satellite this variable is assigned to (if any).
        SatID getSatellite() const
        { return varSat; };


        /** Set internal satellite this variable is assigned to.
         *
         * @param satellite  Internal, specific SatID of variable.
         */
        Variable& setSatellite(const SatID& satellite)
        { varSat = satellite; return (*this); };


        /// Get index of current epoch
        int getNowIndex() const
        { return m_now_index; };


        /// Set index of current epoch
        void  setNowIndex(int index)
        { m_now_index = index; };


        /// Get index of previous epoch
        int getPreIndex() const
        { return m_pre_index; };


        /// Set index of previous epoch
        void setPreIndex( int index)
        { m_pre_index = index; };


        /// Equality operator
        virtual bool operator==(const Variable& right) const;


        /// This ordering is somewhat arbitrary, but is required to be able
        /// to use a Variable as an index to a std::map, or as part of a
        /// std::set.
        virtual bool operator<(const Variable& right) const;


        /// Inequality operator
        bool operator!=(const Variable& right) const
        { return !(operator==(right)); }


        /// Assignment operator
        virtual Variable& operator=(const Variable& right);


        /// SourceID object representing all sources : type(Unknown),
        /// sourceName("").
        static SourceID allSources;


        /// SourceID object representing "some" sources : type(Mixed),
        /// sourceName("").
        static SourceID someSources;


        /// SatID object representing no satellites:
        /// system(systemUnknown), id(-1).
        static SatID noSats;


        /// SatID object representing all satellites:
        /// system(systemMixed), id(-1).
        static SatID allSats;


        /// SatID object representing all satellites of GPS System:
        /// system(systemGPS), id(-1).
        static SatID allGPSSats;


        /// SatID object representing all satellites of Galileo System:
        /// system(systemGalileo), id(-1).
        static SatID allGalileoSats;


        /// SatID object representing all satellites of GLONASS System:
        /// system(systemGLONASS), id(-1).
        static SatID allGlonassSats;


        /// SatID object representing all satellites of BDS System:
        /// system(systemBDS), id(-1).
        static SatID allBDSSats;


        /// Default stochastic model to be assigned to variables.
        static WhiteNoiseModel2 defaultModel;


        /// Destructor
        virtual ~Variable() {};


    private:

        /// Type of the variable
        TypeID varType;

        /// Internal source this variable is assigned to (if any).
        SourceID varSource;

        /// Internal satellite this variable is assigned to (if any).
        SatID varSat;


        /// Pointer stochastic model applying to variable
        StochasticModel2* pVarModel;


        /** Whether this variable is or not source-specific. By default
         *  it is true, which means that this variable is different for
         *  each source (p.e., the TypeID::dx position variable of a given
         *  station).
         */
        bool isSourceIndexed;


        /** Whether this variable is or not satellite-specific. By default
         *  it is false, which means that this variable is same for each
         *  satellite (p.e., the TypeID::dx position variable of a given
         *  station).
         *
         *  On the other hand, if this variable is satellite-specific (i.e.,
         *  this field is true), this means that this variable is different
         *  for each satellite; for instance, the TypeID::dtSat satellite
         *  clock offset variable.
         */
        bool isSatIndexed;


        /** Whether this variable is or not type-specific. By default
         *  it is true, which means that this variable is different for
         *  each observation type (p.e., the TypeID::BL1 and TypeID::BL2
         *  variable of L1 and L2).
         *
         *  On the other hand, if this variable is not type-specific (i.e.,
         *  this field is false), this means that this variable is same for
         *  each observation type; for instance, the TypeID::ionL1 and the
         *  TypeID::ionL2.
         */
        bool isTypeIndexed;


        /// Value of initial variance assigned to this variable.
        double initialVariance;


        /// Value of default coefficient assigned to this variable.
        double defaultCoefficient;


        /// Boolean indicating if default coefficient is always used.
        bool forceDefault;


        /// the index of this Variable in previous VariableSet.
        int m_pre_index;


        /// the index of this Variable in current VariableSet.
        int m_now_index;


        /** Initializing function
         *
         * @param type        TypeID of variable.
         * @param pModel      Pointer to StochasticModel associated with
         *                    this variable. By default, it is a white
         *                    noise model.
         * @param variance    Initial variance assigned to this variable.
         * @param coef        Default coefficient assigned.
         * @param forceCoef   Always use default coefficient.
         */
        void Init( const TypeID& type,
                   StochasticModel2* pModel = &Variable::defaultModel,
                   double variance = 4.0e14,
                   double coef     = 1.0,
                   bool forceCoef  = false,
                   bool typeIndex  = true,
                   int now_index   = -1,
                   int pre_index   = -1);


    }; // End of class 'Variable'


    /// A structure used to store the coefficent information for a Variable.
    /// created by shjzhang
    struct Coefficient
    {
        /// Default constructor
        Coefficient( bool forceCoef   = false,
                     double coef      = 1.0  )
            : forceDefault(forceCoef), defaultCoefficient(coef)
        {};


        /// Boolean indicating if default coefficient is always used.
        bool forceDefault;


        /// Value of default coefficient assigned to this variable.
        double defaultCoefficient;


        /// Equality operator
        bool operator==(const Coefficient& right) const
        {
            return ( ( defaultCoefficient == right.defaultCoefficient ) &&
                     ( forceDefault == right.forceDefault ) );
        }


        /// Destructor
        virtual ~Coefficient() {};

    };


    /// Handy type definition

    typedef std::vector<Variable> VariableVector;

    typedef std::set<Variable> VariableSet;

    typedef std::list<Variable> VariableList;

    typedef std::map<Variable, double> VariableDataMap;

    typedef std::map<Variable, Coefficient> VarCoeffMap;


    namespace StringUtils
    {
        inline std::string asString(const Variable& v)
        {
            std::ostringstream oss;
            oss << v.getType() << "   "
                << v.getSource() << "   "
                << v.getSatellite() << "   "
                << v.getTypeIndexed() << " "
                << v.getSourceIndexed() << " "
                << v.getSatIndexed();

            return oss.str();
        }
    }

    //@}

}  // End of namespace gpstk

#endif   // GPSTK_VARIABLE_HPP
