package com.spotreba.backend;

import jakarta.persistence.*;
import java.time.LocalDateTime;

@Entity
@Table(name = "spotreba_domu")
public class Spotreba {

    @Id
    private Integer id;

    private LocalDateTime datum;
    private Float vykon_w;
    private Float spotreba_kwh;
    private Float napeti_v;
    private Float proud_a;
    private Integer zarizeni_id;

    // GETTERY
    public Integer getId() {
        return id;
    }

    public LocalDateTime getDatum() {
        return datum;
    }

    public Float getVykon_w() {
        return vykon_w;
    }

    public Float getSpotreba_kwh() {
        return spotreba_kwh;
    }

    public Float getNapeti_v() {
        return napeti_v;
    }

    public Float getProud_a() {
        return proud_a;
    }

    public Integer getZarizeni_id() {
        return zarizeni_id;
    }
}
