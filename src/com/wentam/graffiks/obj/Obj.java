package com.wentam.graffiks.obj;

import com.wentam.graffiks.GraffiksRenderer;
import com.wentam.graffiks.mesh.Mesh;
import com.wentam.graffiks.material.Material;

import java.util.ArrayList;

// An Obj is a collection of Mesh objects.
// If provided, will use collection of Material objects when drawing.
public class Obj {
    private GraffiksRenderer renderer;
    private ArrayList<Mesh> meshes;
    private ArrayList<Material> materials;
    private Material default_material;

    private float angleX = 0f;
    private float angleY = 0f;
    private float angleZ = 0f;

    private float locationX = 0f;
    private float locationY = 0f;
    private float locationZ = 0f;

    public Obj (GraffiksRenderer renderer) {
	this.renderer = renderer;
	default_material = new Material(this.renderer.context);
	meshes = new ArrayList<Mesh>();
	materials = new ArrayList<Material>();
    }

    // meshes
    public void addMesh(Mesh mesh) {
	meshes.add(mesh);
    }

    public Mesh getMesh(int i) {
	return meshes.get(i);
    }

    // materials
    public void addMaterial(Material material) {
	materials.add(material);
    }

    public Material getMaterial(int i) {
	return materials.get(i);
    }

    public void draw() {
	int i = 0;
	for (Mesh mesh : meshes) {
	    if (materials.size() >= i+1) {
		mesh.draw(materials.get(i));
	    } else {
		mesh.draw(default_material);
	    }
	    i++;
	}
    }

    public void setAngleDegrees(float x, float y, float z) {
	angleX = x;
	angleY = y;
	angleZ = z;

	for (Mesh mesh : meshes) {
	    mesh.setAngleDegrees(x,y,z);
	}
    }

    public void setLocation(float x, float y, float z) {
	locationX = x;
	locationY = y;
	locationZ = z;
	
	for (Mesh mesh : meshes) {
	    mesh.setLocation(x,y,z);
	}
    }

    public float getAngleDegreesX() {
	return angleX;
    }

    public float getAngleDegreesY() {
	return angleY;
    }

    public float getAngleDegreesZ() {
	return angleZ;
    }


    public float getLocationX() {
	return locationX;
    }

    public float getLocationY() {
	return locationY;
    }

    public float getLocationZ() {
	return locationZ;
    }
}