// anim.h
//
// Copyright 2018 Ben Dooks <ben@fluff.org>

class AnimState {
  public:
  virtual void updateState(void);
  virtual String longName(void);
  virtual String shortName(void);
};

class ChaserAnim : public AnimState {
  public:
    ChaserAnim(RgbColor *pix, int nr_point);
    void updateState(void);

    String longName() { return "Simple Flashing chaser"; };
    String shortName() { return "chaser1"; };

  private:
    RgbColor *pix;
    int nrpix;
    int flpix;
};

class ChaserAnim2 : public AnimState {
  public:
    ChaserAnim2(RgbColor *pix, int nr_point);
    void updateState(void);

    String longName() { return "Simple Flashing chaser (RGB)"; };
    String shortName() { return "chaser2"; };

  private:
    RgbColor *pix;
    int nrpix;
    int flpix;
    unsigned nxcol;
    
    u8_t f_r, f_g, f_b;
    u8_t a_r, a_g, a_b;
};

class NoAnim : public AnimState {
  public:
    NoAnim(RgbColor *pix, int nr_point) { };
    void updateState(void) { };

    String longName() { return "None"; };
    String shortName() { return "none"; };
};

class ChaserAnimOverlay : public AnimState {
  public:
    ChaserAnimOverlay(RgbColor *pix, int nr_point);
    void updateState(void);

    String longName() { return "Chaser"; };
    String shortName() { return "chaser"; };

  private:
    RgbColor *pix;
    int nrpix;
    unsigned  flpix;
};

class SineWaveAnim : public AnimState {
  public:
    SineWaveAnim(RgbColor *pix, int nr_point, String shrt, String lng, u8_t r, u8_t g, u8_t b);
    void updateState(void);

    String longName() { return this->longname; };
    String shortName() { return this->shortname; };

  private:
    RgbColor *pix;
    int nrpix;

    u8_t r, g, b;
    String longname;
    String shortname;
    
    double angle;
};

